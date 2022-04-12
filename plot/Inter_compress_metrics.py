import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FuncFormatter


namelist=["M4","P4","G4"]

#data
y1=[27312,18947,43372]
y2=[30704,20059,501170]

#y3=[14,9,-5]
#y4=[9,5,-6]

y5=[11,10,8]
y6=[5,7,4]

y7=[49,51,24]
y8=[28,47,24]

y9=[35,39,34]
y10=[21,35,21]

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
ax0 = fig.add_subplot(141)
#ax1 = fig.add_subplot(152)
ax2 = fig.add_subplot(142)
ax3 = fig.add_subplot(143)
ax4 = fig.add_subplot(144)

#bar
ax0.set_ylabel('Average Value',axis_font)
rects1 = ax0.bar(ind-width, y1, width,label="Baseline")
rects2 = ax0.bar(ind, y2, width,label="LAMP")
ax0.set_xticks(ind-width/2)
ax0.set_xticklabels(namelist)
ax0.set_xlabel("(a) ZeroCnt",{'family' : 'Times New Roman', 'size'   : 30})
#ax0.set_yscale('log')
#ax0.set_ylim([0,900000])
def formatnum(x, pos):
    if x==0:
        return '0'
    return '$%.1f$x$10^{4}$' % (x/100000)
formatter = FuncFormatter(formatnum)
ax0.yaxis.set_major_formatter(formatter)
'''
rects1 = ax1.bar(ind-width, y3, width, color='g',hatch='.',label="Baseline")
rects2 = ax1.bar(ind, y4, width, color='r',hatch='+',label="LAMP")
ax1.set_xticks(ind-width/2)
ax1.set_xticklabels(namelist)
'''
rects1 = ax2.bar(ind-width, y5,width,label="Baseline")
rects2 = ax2.bar(ind, y6,width,label="LAMP")
ax2.set_xticks(ind-width/2)
ax2.set_xticklabels(namelist)
ax2.set_xlabel("(b) MaxPrec",{'family' : 'Times New Roman', 'size'   : 30})

rects1 = ax3.bar(ind-width, y7, width,label="Baseline")
rects2 = ax3.bar(ind, y8, width,label="LAMP")
ax3.set_xticks(ind-width/2)
ax3.set_xticklabels(namelist)
ax3.set_xlabel("(c) BitsBitplane",{'family' : 'Times New Roman', 'size'   : 30})


rects1 = ax4.bar(ind-width, y9, width,label="Baseline")
rects2 = ax4.bar(ind, y10, width,label="LAMP")
ax4.set_xticks(ind-width/2)
ax4.set_xticklabels(namelist)
ax4.set_xlabel("(d) BlockSize",{'family' : 'Times New Roman', 'size'   : 30})

#ax0.legend(loc=1,ncol=3,  prop=font,bbox_to_anchor=(1, 1.35))
handles, labels = ax0.get_legend_handles_labels()
#fig.legend(handles, labels, loc='upper left',ncol=2,borderaxespad = 0,bbox_to_anchor=(0.5, 0.5))
plt.legend( (rects1[0], rects2[0]), ("Baseline","LAMP") ,ncol=2, bbox_to_anchor=(-0.2, 1.35),prop=font)

#plt.tick_params(labelsize=15)
plt.savefig('InterComMat_ZFP.pdf', format='pdf',bbox_inches="tight",pad_inches=0)

#all show
#plt.tight_layout()
#matplotlib.rc('xtick', labelsize=18)
plt.show()
