import matplotlib
import matplotlib.pyplot as plt
import numpy as np


list1 = np.array([1.337450,1.286783,1.305872,2.614316,1.126064,1.286783,
                  1.235509,1.157356,1.222350,1.961230,1.124563,1.157356,
                  2.095630,1.134745,1.393339,1.451916,1.267940,1.133287]) 
list2 = np.array([1.278378,1.241612,1.261737,2.954776,1.161776,1.241612,
                  1.235089,1.154287,1.217596,1.911658,1.129311,1.154287,
                  2.112557,1.141908,1.391989,1.418224,1.262216,1.139284])

length = len(list1)
x = np.arange(length)

namelist = ["M1", "M2", "M3", "M4", "M5", "M6","P1", "P2", "P3", "P4", "P5", "P6" , "G1", "G2", "G3", "G4", "G5", "G6"]
fig=plt.figure(num=None,figsize=(16,4))
total_width, n = 0.8, 2
width = total_width / n
x1 = x - width / 2
x2 = x + width / 2
axis_font = {'size':'18'}
plt.rc('xtick', labelsize = 18)
plt.rc('ytick', labelsize = 18)
font = {'size':'32'}
ax = fig.add_subplot(111)
ax.set_xticks(x+width)
ax.set_xlim([-0.5,17.5])
#plt.title("FPC compress ratio")
plt.ylabel("Compression ratio", fontsize = 24)
plt.bar(x1, list1, width = width, label = "Baseline")
plt.bar(x2, list2, width = width, label = "LAMP")

plt.xticks(x, namelist)
plt.legend(fontsize = 18)
#plt.show()
plt.savefig('CR_FPC.pdf', format='pdf',bbox_inches="tight",pad_inches=0)
