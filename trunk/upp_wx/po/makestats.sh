#!/bin/bash
IFS_OLD=$IFS
IFS=:
LANGS=(ar:bs:cs:de:el:es:fr:he:hu:it:ja:ms:nl:pa:pt:ro:ru:tr:uk)

LANGNAMES=(Arabic:Bosnian:Czech:German:Greek:Spanish:French:Hebrew:Hungarian:Italian:Japanese:Malay:Dutch:Punjabi:Portuguese:Romanian:Russian:Turkish:Ukrain)

TRANSLATORS=("Hadi Saoud":"Igor Prša":"Jiri Merth":"Bernd Kernbaum":"Jim Black":"Cristian Forcadell Ayza and Diego Herranz":"Patrick Nies":"Sephy":"Molnár Károly":"Giovanni Lorandini and Nils Fredrik Ciancio":"Syuuki Taranari":"Hafidzuddin Mazlan":"Frans Schreuder":"Kanwaljit Singh":"Fábio Silva, Lucas Paulino, Azevedo and Vinícius Pais de Oliveira":"Vlad Praja":"Serafim Nenarokov":"Salih Palamut":"Roman Stets")

echo "<tr><td><strong>Language:</strong></td><td><strong>Credits:</strong></td><td><strong>Status:</strong></td></tr>"
for LANG in $LANGS
do


echo $LANG

done

for TRANSLATOR in $TRANSLATORS
do
echo $TRANSLATOR
done

IFS=$IFS_OLD

