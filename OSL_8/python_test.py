import subprocess
import matplotlib.pyplot as plt 
# Run the compiled C++ program
lisf = []
lisl = []
lisr = []
x = list(range (10, 31, 5))
for mem in x:
    process = subprocess.Popen(["./main","100", str(mem), "50", "./data/req1.dat"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = process.communicate()

    # Decode the output from bytes to string
    output_str = output.decode()

    # Print the output of the C++ program
    print("Output of the C++ program:")
    print(output_str)
    output_str = output_str.split()  
    fifo, lru, rn = int(output_str[3]), int(output_str[7]), int(output_str[11])
    print(fifo, lru, rn)

    lisf.append(fifo)
    lisl.append(lru)
    lisr.append(rn)



assert len(x) == len(lisf) == len(lisl) == len(lisr), "Lists must have the same length"

plt.plot(x, lisf, marker='*')
plt.plot(x, lisl, marker='+')
plt.plot(x, lisr, marker='o')

plt.xlabel("Main Memory Capacity (in pages)")
plt.ylabel("Page Faults")

plt.legend(["FIFO", "LRU", "RANDOM"])
# plt.title("Request 1: Random")

plt.grid(True)

plt.show()
