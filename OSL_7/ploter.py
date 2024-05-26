import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("output.csv", header=None, index_col=None)
# print(df.head())
print(df.columns)
plt.plot(df[0].values, df[1].values)
plt.xlabel("limit")
plt.ylabel("fraction of valid addresses")

plt.show()