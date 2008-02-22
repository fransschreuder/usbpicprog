template=`find . -name "*.pot" | sed -e 's#^./\(.*\)$#\1#'`
files=`find . -name "*.po" | sed -e 's#^./\(.*\)$#\1#'`
test -n "$VERBOSE" && echo $files

for file in $files; do
  echo merging $file
  files=
  if test ! -s $file; then
    echo "ERROR: $file is empty!"
  elif msgmerge --no-wrap -q -o $file $file $template; then
    files="$files $file"
  else
    echo "ERROR: msgmerge failed for $file"
  fi
done
