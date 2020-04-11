import re

class Node:
    def __init__(self, op, *children):
        self.op = op
        self.children = children
    
    def _str(self, depth):
        r = ' ' * depth + self.op + '\n'
        for child in self.children:
            r += child._str(depth + 1)
        return r

    def __str__(self):
        return self._str(0)

"""
Transforms a string into a tree, for any number of (single
character) binary operators. Token names may not have spaces
or characters that are binary operators.

Example:

"axe + myfunc(x, y * z ^ 5) < 7"

   <
7          +
      axe      myfunc
                  ,
              x         *
                     y     ^
                          z 5
"""
def str2tree(text, ops=[',', '<=>', '+-', '*/%', '^']):
    text = text.strip()
    depth = 0
    depths = []
    for c in text:
        if c == '(':
            depth += 1
        elif c == ')':
            depth -= 1
        depths.append(depth)
    zeros = [i for i in range(len(depths)) if depths[i] == 0]

    # Strip outer-most parentheses.
    if len(zeros) == 1 and len(text) > 1:
        assert zeros[0] == len(text) - 1
        return str2tree(text[1:-1])
    
    # Parse operations in reverse-priority order from the
    # back of the string to the front.
    for op in ops:
        for z in zeros[::-1]:
            if text[z] in op:
                return Node(
                    text[z],
                    str2tree(text[:z]),
                    str2tree(text[z+1:])
                )

    # This expression is either a variable or a function
    # call.
    funcname = re.findall(r"^[^\( ]+\(", text)
    if len(funcname) > 0:
        print(funcname)
        n = len(funcname[0]) - 1
        return Node(text[:n], str2tree(text[n:]))
    
    if ' ' in text:
        raise Exception(f'Unexpected token "{text}"')
    
    return Node(text)
