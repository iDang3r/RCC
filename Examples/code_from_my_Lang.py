from math import *
t = 0
r = 0
p = 0
d = 0
q = 0
x = 0
y = 0
def l(b, c):
  global t
  global r
  global p
  global d
  global q
  global x
  global y
  if b == 0:
    if c == 0:
      return -1
    else:
      return 0

  else:
    x = ((-1 * c) // b)
    return 1

def s(a, b, c):
  global t
  global r
  global p
  global d
  global q
  global x
  global y
  if a == 0:
    return l(b, c)
  else:
    d = ((b * b) - ((4 * a) * c))
    if d < 0:
      return 0
    else:
      if d != 0:
        x = (((-1 * b) + sqrt(d)) // (2 * a))
        y = (((-1 * b) - sqrt(d)) // (2 * a))
        return 2
      else:
        x = ((-1 * b) // (2 * a))
        return 1



t = int(input())
r = int(input())
p = int(input())
q = s(t, r, p)
if q == 2:
  print(q)
  print(x)
  print(y)
else:
  if q == 1:
    print(q)
    print(x)
  else:
    print(q)
