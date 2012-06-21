#!/bin/bash


LANGS=("ar" "bs" "cs" "de" "el" "es" "fr" "he" "hu" "it" "ja" "ms" "nl" "pa" "pt" "ro" "ru" "tr" "uk")

LANGNAMES=("Arabic" "Bosnian" "Czech" "German" "Greek" "Spanish" "French" "Hebrew" "Hungarian" "Italian" "Japanese" "Malay" "Dutch" "Punjabi" "Portuguese" "Romanian" "Russian" "Turkish" "Ukrain")

TRANSLATORS=("Hadi Saoud" "Igor Pr&#353;a" "Jiri Merth" "Bernd Kernbaum" "Jim Black" "Cristian Forcadell Ayza and Diego Herranz" "Patrick Nies" "Sephy" "Moln&#225;r K&#225;roly" "Giovanni Lorandini and Nils Fredrik Ciancio" "Syuuki Taranari" "Hafidzuddin Mazlan" "Frans Schreuder" "Kanwaljit Singh" "F&#225;bio Silva, Lucas Paulino, Azevedo and Vin&#237;cius Pais de Oliveira" "Vlad Praja" "Serafim Nenarokov" "Salih Palamut" "Roman Stets")


echo "<table border=\"1\" width=\"100%\"><tbody><tr><td><strong>Language:</strong></td><td><strong>Credits:</strong></td><td><strong>Status:</strong></td></tr>">allstats.htm

count=${#LANGS[@]}
index=0
while [ "$index" -lt "$count" ]; do
	LANG=$(echo -e "${LANGS[$index]}")
	LANGNAME=$(echo -e "${LANGNAMES[$index]}")
	TRANSLATOR=$(echo -e "${TRANSLATORS[$index]}")
	STATS=$(cat $LANG.po.stats)
	STATS=${STATS//", "/<font color=\"#FF0000\"><br>}
	echo -e "<tr><td><a href=\"https://github.com/fransschreuder/usbpicprog/raw/master/trunk/upp_wx/po/$LANG.po\" target=\"_BLANK\">$LANG.po - $LANGNAME</a></td>\n<td>by $TRANSLATOR</td><td>$STATS</font></font></td></tr>" >>allstats.htm	


#    echo -e "index: $index\tvalue: ${LANGS[$index]}"
#    echo -e "index: $index\tvalue: ${TRANSLATORS[$index]}"
    let "index++"




done
echo "</tbody></table>">>allstats.htm
#i=0
#for TRANSLATOR in $TRANSLATORS
#do
#	i=$(($i+1))
#	echo ${TRANSLATORS:1}
#done



