import numpy as np
import matplotlib.pyplot as plt

N1=2000
N2=2048
procList = np.array([1, 2, 4, 8, 16, 32, 64])
matmulTimeData = np.array([16.388896, 13.143288, 6.654653, 3.683206, 2.005205, 1.128678, 0.631951])
partATimeData = np.array([1.924067, 1.255380, 0.680083, 0.538641, 0.599632, 0.949931, 1.539906])
partBTimeData = np.array([1.929339, 1.651401, 0.585626, 0.422022, 0.270074, 0.245653, 0.208228])
index = np.arange(len(procList))

SU_matmulTimeData = np.array([matmulTimeData[0]/time for time in matmulTimeData])
SU_partATimeData = np.array([partATimeData[0]/time for time in partATimeData])
SU_partBTimeData = np.array([partBTimeData[0]/time for time in partBTimeData])

plt.figure()
plt.plot(index, matmulTimeData, 'r--')
plt.ylabel('Seconds')
plt.xlabel('Threads')
plt.xticks(index, procList)
plt.title('Time in seconds vs Number of Threads\nmpi_matrixmul.c')

plt.figure()
plt.plot(index, SU_matmulTimeData, 'r--')
plt.ylabel('Speed Up')
plt.xlabel('Threads')
plt.xticks(index, procList)
plt.title('Speed Up vs Number of Threads\nmpi_matrixmul.c')


plt.figure()
plt.plot(index, partATimeData, 'r--', label='partA.c')
plt.plot(index, partBTimeData, 'b--', label='partB.c')
plt.ylabel('Seconds')
plt.xlabel('Threads')
plt.xticks(index, procList)
plt.title('Time in seconds vs Number of Threads\nQ2')
plt.legend(loc='upper left')

plt.figure()
plt.plot(index, SU_partATimeData, 'r--', label='partA.c')
plt.plot(index, SU_partBTimeData, 'b--', label='partB.c')
plt.ylabel('Speed Up')
plt.xlabel('Threads')
plt.xticks(index, procList)
plt.title('Speed Up vs Number of Threads\nQ2')
plt.legend(loc='upper left')

plt.show()