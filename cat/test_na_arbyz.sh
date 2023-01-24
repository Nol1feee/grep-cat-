#!/ bin / bash

SUCCESS=0
FAIL=0
COUNT=0

for var1 in b e n s t v
do
	for var2 in b e n s t v 
	do
		for var3 in b e n s t v 
		do
			for var4 in b e n s t v
			do	
				cat -$var1 -$var2 -$var3 -$var4 *.txt > orig_cat.log
				./s21_cat -$var1 -$var2 -$var3 -$var4 *.txt > s21_cat.log
				(( COUNT++ ))
				if [ "$(diff -s orig_cat.log s21_cat.log)" == "Files orig_cat.log and s21_cat.log are identical" ]
				then
					(( SUCCESS++ ))
					echo "COUNT: $COUNT SUCCESS: $SUCCESS"
				else 
					(( FAIL++ ))
					echo "COUNT: $COUNT FAIL: $FAIL"
				fi
			done
		done
	done
done

for var1 in b e n s t v
do
	for var2 in b e n s t v 
	do
		cat -$var1 -$var2 one.txt > orig_cat.log
		./s21_cat -$var1 -$var2 one.txt > s21_cat.log
		(( COUNT++ ))
		if [ "$(diff -s orig_cat.log s21_cat.log)" == "Files orig_cat.log and s21_cat.log are identical" ]
		then
			(( SUCCESS++ ))
			echo "COUNT: $COUNT SUCCESS: $SUCCESS"
		else 
			(( FAIL++ ))
			echo "COUNT: $COUNT FAIL: $FAIL"
		fi
	done
done

for var1 in b e n s t v
do
	for var2 in b e n s t v 
	do
		cat -$var1 -$var2 two.txt > orig_cat.log
		./s21_cat -$var1 -$var2 two.txt > s21_cat.log
		(( COUNT++ ))
		if [ "$(diff -s orig_cat.log s21_cat.log)" == "Files orig_cat.log and s21_cat.log are identical" ]
		then
			(( SUCCESS++ ))
			echo "COUNT: $COUNT SUCCESS: $SUCCESS"
		else 
			(( FAIL++ ))
			echo "COUNT: $COUNT FAIL: $FAIL"
		fi
	done
done

echo "\n\n\n##################"
echo "TOTAL COUNT: $COUNT"
echo "TOTAL SUCCESS: $SUCCESS"
echo "TOTAL FAIL: $FAIL"
echo "##################"

