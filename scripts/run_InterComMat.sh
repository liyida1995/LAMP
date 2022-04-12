cd ../plot
List1="M4 P4 G4"
List2="Base LAMP"
set -- $List1
for j
do
 set -- $List2
 for i
 do
        echo $err

        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        echo $j"_"$i
        echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
	Z=$j"_ZeroCnt_"$i
        Me=$j"_MaxExp_"$i
	Mp=$j"_MaxPrec_"$i
	Bits=$j"_BitsBitplane_"$i
	Block=$j"_BlockSize_"$i
        
        python3 Inter_compress_metrics_pre.py $Z $Me $Mp $Bits $Block >>ZFP_InterComMat.cnt
  done
  echo "##########  $j  done  ##########"
done


