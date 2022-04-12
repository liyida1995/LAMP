cd ../plot
List1="0 1 4 7 10 15"
List2="0 1 4 7 10 16"
set -- $List1
for i
do
        #    echo $err

        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        echo $i
        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        O=ori_Pin_$i.bcode
        L=LAMP_Pin_$i.bcode
        python3 bcode_plot.py $O 
	python3 bcode_plot.py $L
done
set -- $List1
for i
do
        #    echo $err

        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        echo $i
        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        O=ori_Min_$i.bcode
        L=LAMP_Min_$i.bcode
        python3 bcode_plot.py $O
        python3 bcode_plot.py $L
done

set -- $List2
for i
do
        #    echo $err

        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        echo $i
        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        O=ori_Green_$i.bcode
        L=LAMP_Green_$i.bcode
        python3 bcode_plot.py $O
        python3 bcode_plot.py $L
done


