#!/bin/bash
xgettext --files-from=po/POTFILES.in --default-domain=usbpicprog --keyword=_ -C -s

mv usbpicprog.po po/input.pot
cd po
msgmerge usbpicprog.pot input.pot > usbpicprog.pot.tmp
mv usbpicprog.pot.tmp usbpicprog.pot

for file in *.po
do
  echo "Processing $file file..."
  msgmerge $file input.pot > $file.tmp
  mv $file.tmp $file
  msgfmt $file -o ${file:0:2}.mo
  msgfmt --statistics $file > $file.stats 2>&1
done
#-x po/usbpicprog.pot
