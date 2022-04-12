import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter


namelist=["M4","P4","G4"]

#data
#QuantIntv
y1=[65536,8192,2048]
y2=[65536,8192,1024]
#HitRatio
y3=[0.952151,0.982708,0.990632]
y4=[0.975054,0.989396,0.991817]
#OutSize
y5=[105060,290249,2004949]
y6=[89235,284653,1872418]
#treeByteSize
y7=[53056,125488,36856]
y8=[33004,120412,18424]
#enCodeSize
y9=[51996,164753,1968085]
y10=[56223,164233,1853986]

x=range(0,3)
N = len(x)
print(N)
ind = np.arange(N)  # the x locations for the groups
width = 0.27       # the width of the bars
font = {'size':'30'}
axis_font = {'size':"30"}

plt.rc('xtick',labelsize=24)
plt.rc('ytick',labelsize=20)

#subplot
fig=plt.figure(num=None,figsize=(20,4))
#plt.subplots_adjust(hspace=0.3, wspace=0.3)
'''plt.subplots_adjust(left=0.125,
                    bottom=0.1, 
                    right=0.9, 
                    top=0.9, 
                    wspace=0.2, 
                    hspace=0.35)
'''
#ax0 = fig.add_subplot(151)
ax1 = fig.add_subplot(141)
ax2 = fig.add_subplot(142)
ax3 = fig.add_subplot(143)
ax4 = fig.add_subplot(144)

#bar
ax1.set_ylabel('Average Value',axis_font)
'''
rects1 = ax0.bar(ind-width, y1, width, color='g',hatch='.',label="Baseline")
rects2 = ax0.bar(ind, y2, width, color='r',hatch='+',label="LAMP")
ax0.set_xticks(ind-width/2)
ax0.set_xticklabels(namelist)
#ax0.set_yscale('log')
#ax0.set_ylim([0,900000])
def formatnum(x, pos):
    return '$%.1f$x$10^{4}$' % (x/100000)
formatter = FuncFormatter(formatnum)
ax0.yaxis.set_major_formatter(formatter)
'''
rects1 = ax1.bar(ind-width, y3, width,label="Baseline")
rects2 = ax1.bar(ind, y4, width,label="LAMP")
ax1.set_xticks(ind-width/2)
ax1.set_xticklabels(namelist)
ax1.set_xlabel("(a) Hitratio",{'family' : 'Times New Roman', 'size'   : 30})

rects1 = ax2.bar(ind-width, y5, width,label="Baseline")
rects2 = ax2.bar(ind, y6, width,label="LAMP")
ax2.set_xticks(ind-width/2)
ax2.set_xticklabels(namelist)
#ax2.set_ylim([0,700000])
#ax2.set_yscale('log')
def formatnum(x, pos):
    if x==0:
        return '0'
    return '$%.1f$x$10^{4}$' % (x/100000)
formatter = FuncFormatter(formatnum)
ax2.yaxis.set_major_formatter(formatter)
ax2.set_xlabel("(b) OutlierSize",{'family' : 'Times New Roman', 'size'   : 30})

rects1 = ax3.bar(ind-width, y7, width,label="Baseline")
rects2 = ax3.bar(ind, y8, width,label="LAMP")
ax3.set_xticks(ind-width/2)
ax3.set_xticklabels(namelist)
#ax3.set_yscale('log')

def formatnum(x, pos):
    if x==0:
        return '0'
    return '$%.1f$x$10^{4}$' % (x/100000)
formatter = FuncFormatter(formatnum)
ax3.yaxis.set_major_formatter(formatter)
ax3.set_xlabel("(c) TreeSize",{'family' : 'Times New Roman', 'size'   : 30})

rects1 = ax4.bar(ind-width, y9, width,label="Baseline")
rects2 = ax4.bar(ind, y10, width,label="LAMP")
ax4.set_xticks(ind-width/2)
ax4.set_xticklabels(namelist)
#ax4.set_yscale('log')
#ax4.set_ylim([0,700000])

def formatnum(x, pos):
    if x==0:
        return '0'
    return '$%.1f$x$10^{4}$' % (x/100000)
formatter = FuncFormatter(formatnum)
ax4.yaxis.set_major_formatter(formatter)
ax4.set_xlabel("(d) EncodeSize",{'family' : 'Times New Roman', 'size'   : 30})

handles, labels = ax1.get_legend_handles_labels()
plt.legend( (rects1[0], rects2[0]), ("Baseline","LAMP") ,ncol=2, bbox_to_anchor=(-0.5, 1.35),prop=font)
plt.subplots_adjust(left=None, bottom=None, right=None, top=None, hspace=None, wspace=0.5)


plt.savefig('InterComMat_SZ.pdf', format='pdf',bbox_inches="tight",pad_inches=0)

#all show
#plt.tight_layout()
plt.show()
