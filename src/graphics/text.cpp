#include "text.hpp"

struct Character {
	char	   value;
	GLuint     TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing; // offset from baseline to top/left of glyph (..height)
	GLuint     Advance;		// offset to next glyph
};

static GLuint TEXT_SHADER;
static GLuint VAO, VBO;
static glm::mat4 text_projection;
static std::map<GLchar, Character> Characters;

void TextInit() {
	// Interface
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		fprintf(stderr, "ERROR::FREETYPE: Could not initialise the FreeType library.\n");
		exit(1);
	}
	FT_Face face;
	if (FT_New_Face(ft, "assets/fonts/calibri.ttf", 0, &face)) {
		fprintf(stderr, "ERROR::FREETYPE: Could not load font.\n");
		exit(1);
	}
	FT_Set_Pixel_Sizes(face, 0, 24);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			fprintf(stderr, "ERROR::FREETYPE: Failed to load Glyph: %i.\n", c);
		}
		// Generate Texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Character character = {
			c,
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	TEXT_SHADER = BuildGlProgram("./src/shaders/text_v_shader.glsl",
								 "./src/shaders/text_f_shader.glsl");
	glUseProgram(TEXT_SHADER);
	printf("INIT::TEXT_SHADER: %i\n", TEXT_SHADER);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create Text Buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextUpdate(int width, int height) {
	text_projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
	
}

void TextDraw(char *text_data, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
	// Activate corresponding render state
	glUseProgram(TEXT_SHADER);
	glUniform3f(glGetUniformLocation(TEXT_SHADER, "textColor"), color.x, color.y, color.z);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	glUniformMatrix4fv(glGetUniformLocation(TEXT_SHADER, "projection"), 1, GL_FALSE, &text_projection[0][0]);

	std::string text = std::string(text_data);
	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}