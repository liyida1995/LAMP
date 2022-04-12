import numpy as np
import sys
import array
import os

if len(sys.argv) < 1 :
    print('usage: python %s <file.dat' % sys.argv[0])
    sys.exit(0)
#statinfo=os.stat(sys.argv[1])
#fsize=statinfo.st_size
numbers_int=[]
mean=[]

zero=0
#compute average of 5 files
for i in range(1,6):
    with open(sys.argv[1], 'r') as f:
        data=f.readlines()            # loadtext all info_str
        count=len(data)                #compute rowcnt of text
        print(count)  
        for line in data:
            odom = line.strip('\n')      # move \n  
            #if int(odom)==0:
             #   zero+=1
            numbers_int.append(int(odom))  # transform to int
     
    dataset=np.array(numbers_int)
    if sys.argv[1][3:4]=="Z":
        mean.append(count)
    else:
        mean.append(int(np.sum(dataset)/count))
   # print(dataset)
   # print(mean)
    #print(zero)
    numbers_int=[]
print(mean)


