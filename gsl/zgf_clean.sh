# get all dictionary 
all_dicts=`ls -d */`

rm -f *.gcno *.bc

for dict in $all_dicts:
do
  work_dic=`echo ${dict%%:*}`
  
  # enter working dictionary
  cd ${work_dic}
  echo "==== Enter ==== > "${PWD}

  rm -f *.gcno *.bc

  cd ..
done
