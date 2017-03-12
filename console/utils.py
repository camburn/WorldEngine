"""
Basic utilities
"""

def load_file(filepath):
    """
    Return the contents of an external file.

    Args:
        filepath (str): Path and name of file to open
    """
    try:
        with open(filepath, 'r') as f:
            data = f.read()
    except FileNotFoundError:
        print(f'Cannot open {filepath}')
        return None
    return data
