cd ../plot
List1="0 1 4 7 10 15"
List2="0 1 4 7 10 16"

set -- $List1
for i
do
	    echo $err

	echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
	echo $i
	echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        O=ori_Pin_$i.count
	L=LAMP_Pin_$i.count
	name=Pin_count_$i
	python3 outcnt_plot.py $name $O $L 
done
#
####arrange(2)####
#for i
#do
#        echo $err
#
#        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#        echo $i
#        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#        O=ori_Min_$i.count
#        L=LAMP_Min_$i.count
#        name=Min_count_$i
#        python3 outcnt_plot.py $name $O $L
#done

##lineaspace(x1,x2,6)##
#set -- $List2
#for i
#do
#        echo $err
#
#        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#        echo $i
#        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#        O=ori_Green_$i.count
#        L=LAMP_Green_$i.count
#        name=Green_count_$i
#        python3 outcnt_plot.py $name $O $L
#done

