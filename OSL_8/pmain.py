import random


if __name__ == "__main__":
    print(random.randint(1, 100))
    
    # lis = [random.randint(1, 50) for range ]
    print(*[_ for _ in range(1, 51)])
    print(*[51 - _ for _ in range(1, 51)])
    