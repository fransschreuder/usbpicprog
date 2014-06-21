DATE=$(date +"%y%m%d")
DATE=${DATE:0:6}
if [ 1 ]; then
  ( echo '// generated file';
  printf '#define SVN_REVISION "usbpicprog ';
  echo -n $DATE;
  echo '"' ) > svn_revision.h.new;
  if [ ! -f svn_revision.h ]; then
    mv -f svn_revision.h.new svn_revision.h;
  else
    if cmp svn_revision.h svn_revision.h.new; then
      rm -f svn_revision.h.new;
    else
      mv -f svn_revision.h.new svn_revision.h;
    fi
  fi
fi
if [ ! -f svn_revision.h ]; then
  ( echo '// generated file';
    echo '#define SVN_REVISION "usbpicprog 1.0.0"'; ) > svn_revision.h;
fi
