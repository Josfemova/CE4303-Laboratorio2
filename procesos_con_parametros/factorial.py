import sys

def factorial(x):
    if x == 0:
        return 1
    return x*factorial(x-1)


if len(sys.argv) != 2:
    print("Uso: python factorial.py [número]")
    sys.exit(1)

try:
    number = int(sys.argv[1])
except ValueError as e:
    print("Por favor provea un entero válido como input.")

if number < 0:
    print("El factorial está indefinido para números negativos.")
else:
    if number + 10 > sys.getrecursionlimit():
        sys.setrecursionlimit(number + 10)
    result = factorial(number)
    try: 
        print(f"El factorial de {number} es {result}")
    except ValueError:
        print("El resultado es demasiado grande para imprimir.")

