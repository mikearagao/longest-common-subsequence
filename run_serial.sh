echo "----------------------------------"
echo "Running CPD Project Serial Version"
echo "----------------------------------\n\n"
echo "-------- Start ex1 ---------"
./serial ex1.in > out.ex1.out
diff out.ex1.out ex1.out
echo "-------- End ex1 ---------\n\n"
echo "-------- Start ex10.15 ---------"
./serial ex10.15.in > out.ex10.15.out
diff out.ex10.15.out ex10.15.out
echo "-------- End ex10.15 ---------\n\n"
echo "-------- Start ex150.200 ---------"
./serial ex150.200.in > out.ex150.200.out
diff out.ex150.200.out ex150.200.out
echo "-------- End ex150.200 ---------\n\n"
echo "-------- Start ex3k.8k ---------"
./serial ex3k.8k.in > out.ex3k.8k.out
diff out.ex3k.8k.out ex3k.8k.out
echo "-------- End ex3k.8k ---------\n\n"
echo "-------- Start ex18k.17k ---------"
./serial ex18k.17k.in > out.ex18k.17k.out
diff out.ex18k.17k.out ex18k.17k.out
echo "-------- End ex18k.17k ---------\n\n"
echo "-------- Start ex48k.30k ---------"
./serial ex48k.30k.in > out.ex48k.30k.out
diff out.ex48k.30k.out ex48k.30k.out
echo "-------- End ex48k.30k ---------\n\n"
echo "----------------------------------"
echo "------------ THE END -------------"
echo "----------------------------------"