echo "----------------------------------"
echo "Running CPD Project Serial Version"
echo "----------------------------------\n\n"
echo "-------- Start ex10.15 ---------"
time ../serial/serial ../instances/ex10.15.in > ../results/out.ex10.15.out
diff ../results/out.ex10.15.out ../instances/ex10.15.out
echo "-------- End ex10.15 ---------\n\n"
echo "-------- Start ex150.200 ---------"
time ../serial/serial ../instances/ex150.200.in > ../results/out.ex150.200.out
diff ../results/out.ex150.200.out ../instances/ex150.200.out
echo "-------- End ex150.200 ---------\n\n"
echo "-------- Start ex3k.8k ---------"
time ../serial/serial ../instances/ex3k.8k.in > ../results/out.ex3k.8k.out
diff ../results/out.ex3k.8k.out ../instances/ex3k.8k.out
echo "-------- End ex3k.8k ---------\n\n"
echo "-------- Start ex18k.17k ---------"
time ../serial/serial ../instances/ex18k.17k.in > ../results/out.ex18k.17k.out
diff ../results/out.ex18k.17k.out ../instances/ex18k.17k.out
echo "-------- End ex18k.17k ---------\n\n"
echo "-------- Start ex48k.30k ---------"
time ../serial/serial ../instances/ex48k.30k.in > ../results/out.ex48k.30k.out
diff ../results/out.ex48k.30k.out ../instances/ex48k.30k.out
echo "-------- End ex48k.30k ---------\n\n"
echo "----------------------------------"
echo "------------ THE END -------------"
echo "----------------------------------"