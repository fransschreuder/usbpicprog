// modified from KDE 3.4 for Windows port (Nicolas Hadacek)

/*
   Copyright (C) 1999 Waldo Bastian <bastian@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

//#include <config.h>

#include <sys/param.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include <qdir.h>
#include <qfile.h>
#include <qurl.h>

#include <qstringlist.h>
#if QT_VERSION<0x040000
#  include <qasciidict.h>
#  include <qstrlist.h>
#else
#  include <Qt3Support/Q3StrList>
#  define QGList Q3GList
#  define QStrList Q3StrList
#  include <Qt3Support/Q3AsciiDict>
#  define QGDict Q3GDict
#  define QAsciiDict Q3AsciiDict
#  include <Qt3Support/Q3PtrCollection>
#  define QPtrCollection Q3PtrCollection
#endif


#include "nokde_kcmdlineargs.h"
#include "nokde_kaboutdata.h"
#include "nokde_klocale.h"
//#include <kapplication.h>
//#include <kglobal.h>
//#include <kstringhandler.h>
//#include <kstaticdeleter.h>

#ifdef Q_WS_X11
#define DISPLAY "DISPLAY"
#elif defined(Q_WS_QWS)
#define DISPLAY "QWS_DISPLAY"
#endif

#ifdef Q_WS_WIN
#include <win32_utils.h>
#endif

template class QAsciiDict<QCString>;
template class QPtrList<KCmdLineArgs>;

class KCmdLineParsedOptions : public QAsciiDict<QCString>
{
public:
   KCmdLineParsedOptions()
     : QAsciiDict<QCString>( 7 ) { }

   // WABA: Huh?
   // The compiler doesn't find KCmdLineParsedOptions::write(s) by itself ???
   // WABA: No, because there is another write function that hides the
   // write function in the base class even though this function has a
   // different signature. (obscure C++ feature)
   QDataStream& save( QDataStream &s) const
   { return QGDict::write(s); }

   QDataStream& load( QDataStream &s)
   { return QGDict::read(s); }

protected:
   virtual QDataStream& write( QDataStream &s, QPtrCollection::Item data) const
   {
      QCString *str = (QCString *) data;
      s << (*str);
      return s;
   }

   virtual QDataStream& read( QDataStream &s, QPtrCollection::Item &item)
   {
      QCString *str = new QCString;
      s >> (*str);
      item = (void *)str;
      return s;
   }

};

class KCmdLineParsedArgs : public QStrList
{
public:
   KCmdLineParsedArgs()
     : QStrList( true ) { }
   QDataStream& save( QDataStream &s) const
   { return QGList::write(s); }

   QDataStream& load( QDataStream &s)
   { return QGList::read(s); }
};


class KCmdLineArgsList: public QPtrList<KCmdLineArgs>
{
public:
   KCmdLineArgsList() { }
};

KCmdLineArgsList *KCmdLineArgs::argsList = 0;
int KCmdLineArgs::argc = 0;
char **KCmdLineArgs::argv = 0;
char *KCmdLineArgs::mCwd = 0;
//static KStaticDeleter <char> mCwdd;
const KAboutData *KCmdLineArgs::about = 0;
bool KCmdLineArgs::parsed = false;
bool KCmdLineArgs::ignoreUnknown = false;

//
// Static functions
//

void
KCmdLineArgs::init(int _argc, char **_argv, const char *_appname, const char* programName,
                   const char *_description, const char *_version, bool noKApp)
{
   init(_argc, _argv,
        new KAboutData(_appname, programName, _version, _description),
        noKApp);
}

void
KCmdLineArgs::init(int _argc, char **_argv, const char *_appname,
                   const char *_description, const char *_version, bool noKApp)
{
   init(_argc, _argv,
        new KAboutData(_appname, _appname, _version, _description),
        noKApp);
}

void
KCmdLineArgs::initIgnore(int _argc, char **_argv, const char *_appname )
{
   init(_argc, _argv,
        new KAboutData(_appname, _appname, "unknown", "KDE Application", false));
   ignoreUnknown = true;
}

void
KCmdLineArgs::init(const KAboutData* ab)
{
   char **_argv = (char **) malloc(sizeof(char *));
   _argv[0] = (char *) ab->appName();
   init(1,_argv,ab, true);
}


void
KCmdLineArgs::init(int _argc, char **_argv, const KAboutData *_about, bool /*noKApp*/)
{
   argc = _argc;
   argv = _argv;

   if (!argv)
   {
      fprintf(stderr, "\n\nFAILURE (KCmdLineArgs):\n");
      fprintf(stderr, "Passing null-pointer to 'argv' is not allowed.\n\n");

      assert( 0 );
      exit(255);
   }

   // Strip path from argv[0]
   if (argc) {
     char *p = strrchr( argv[0], '/');
     if (p)
       argv[0] = p+1;
   }

   about = _about;
   parsed = false;
   mCwd = /*mCwdd.setObject(mCwd, */new char [PATH_MAX+1];//, true);
   getcwd(mCwd, PATH_MAX);
#ifdef Q_WS_WIN
   win32_slashify(mCwd, PATH_MAX);
#endif
//   if (!noKApp)
//      KApplication::addCmdLineOptions();
}

QString KCmdLineArgs::cwd()
{
   return QFile::decodeName(QCString(mCwd));
}

const char * KCmdLineArgs::appName()
{
   if (!argc) return 0;
   return argv[0];
}

void
KCmdLineArgs::addCmdLineOptions( const KCmdLineOptions *options, const char *name,
                                 const char *id, const char *afterId)
{
   if (!argsList)
      argsList = new KCmdLineArgsList();

   int pos = argsList->count();

   if (pos && id && argsList->last() && !argsList->last()->name)
      pos--;

   KCmdLineArgs *args;
   int i = 0;
   for(args = argsList->first(); args; args = argsList->next(), i++)
   {
      if (!id && !args->id)
         return; // Options already present.

      if (id && args->id && (::qstrcmp(id, args->id) == 0))
         return; // Options already present.

      if (afterId && args->id && (::qstrcmp(afterId, args->id) == 0))
         pos = i+1;
   }

   assert( parsed == false ); // You must add _ALL_ cmd line options
                              // before accessing the arguments!
   args = new KCmdLineArgs(options, name, id);
   argsList->insert(pos, args);
}

void
KCmdLineArgs::saveAppArgs( QDataStream &ds)
{
   if (!parsed)
      parseAllArgs();

   // Remove Qt and KDE options.
   removeArgs("qt");
   removeArgs("kde");

   QCString qCwd = mCwd;
   ds << qCwd;

   uint count = argsList ? argsList->count() : 0;
   ds << count;

   if (!count) return;

   KCmdLineArgs *args;
   for(args = argsList->first(); args; args = argsList->next())
   {
      ds << QCString(args->id);
      args->save(ds);
   }
}

void
KCmdLineArgs::loadAppArgs( QDataStream &ds)
{
   // Remove Qt and KDE options.
   removeArgs("qt");
   removeArgs("kde");

   KCmdLineArgs *args;
   if ( argsList ) {
      for(args = argsList->first(); args; args = argsList->next())
      {
         args->clear();
      }
   }

   if (ds.atEnd())
      return;

   QCString qCwd;
   ds >> qCwd;
   delete [] mCwd;

   mCwd = /*mCwdd.setObject(mCwd, */new char[qCwd.length()+1];//, true);
   strncpy(mCwd, qCwd.data(), qCwd.length()+1);

   uint count;
   ds >> count;

   while(count--)
   {
     QCString id;
     ds >> id;
     assert( argsList );
     for(args = argsList->first(); args; args = argsList->next())
     {
       if (args->id  == id)
       {
          args->load(ds);
          break;
       }
     }
   }
   parsed = true;
}

KCmdLineArgs *KCmdLineArgs::parsedArgs(const char *id)
{
   KCmdLineArgs *args = argsList ? argsList->first() : 0;
   while(args)
   {
      if ((id && ::qstrcmp(args->id, id) == 0) || (!id && !args->id))
      {
          if (!parsed)
             parseAllArgs();
          return args;
      }
      args = argsList->next();
   }

   return args;
}

void KCmdLineArgs::removeArgs(const char *id)
{
   KCmdLineArgs *args = argsList ? argsList->first() : 0;
   while(args)
   {
      if (args->id && id && ::qstrcmp(args->id, id) == 0)
      {
          if (!parsed)
             parseAllArgs();
          break;
      }
      args = argsList->next();
   }

   if (args)
      delete args;
}

/*
 * @return:
 *  0 - option not found.
 *  1 - option found                    // -fork
 *  2 - inverse option found ('no')     // -nofork
 *  3 - option + arg found              // -fork now
 *
 *  +4 - no more options follow         // !fork
 */
static int
findOption(const KCmdLineOptions *options, QCString &opt,
           const char *&opt_name, const char *&def, bool &enabled)
{
   int result;
   bool inverse;
   int len = opt.length();
   while(options && options->name)
   {
      result = 0;
      inverse = false;
      opt_name = options->name;
      if ((opt_name[0] == ':') || (opt_name[0] == 0))
      {
         options++;
         continue;
      }

      if (opt_name[0] == '!')
      {
         opt_name++;
         result = 4;
      }
      if ((opt_name[0] == 'n') && (opt_name[1] == 'o'))
      {
         opt_name += 2;
         inverse = true;
      }
      if (strncmp(opt.data(), opt_name, len) == 0)
      {
         opt_name += len;
         if (!opt_name[0])
         {
            if (inverse)
               return result+2;

            if (!options->description)
            {
               options++;
               if (!options->name)
                  return result+0;
               QCString nextOption = options->name;
#  if QT_VERSION<0x040000
               int p = nextOption.find(' ');
#else
               int p = QString(nextOption).indexOf(' ');
#endif
               if (p > 0)
                  nextOption = nextOption.left(p);
               if (strncmp(nextOption.data(), "no", 2) == 0)
               {
                  nextOption = nextOption.mid(2);
                  enabled = !enabled;
               }
               result = findOption(options, nextOption, opt_name, def, enabled);
               assert(result);
               opt = nextOption;
               return result;
            }

            return 1;
         }
         if (opt_name[0] == ' ')
         {
            opt_name++;
            def = options->def;
            return result+3;
         }
      }

      options++;
   }
   return 0;
}


void
KCmdLineArgs::findOption(const char *_opt, QCString opt, int &i, bool _enabled, bool &moreOptions)
{
   KCmdLineArgs *args = argsList->first();
   const char *opt_name;
   const char *def;
   QCString argument;
#  if QT_VERSION<0x040000
   int j = opt.find('=');
#else
   int j = QString(opt).indexOf('=');
#endif
   if (j != -1)
   {
      argument = opt.mid(j+1);
      opt = opt.left(j);
   }

   bool enabled = true;
   int result = 0;
   while (args)
   {
      enabled = _enabled;
      result = ::findOption(args->options, opt, opt_name, def, enabled);
      if (result) break;
      args = argsList->next();
   }
   if (!args && (_opt[0] == '-') && _opt[1] && (_opt[1] != '-'))
   {
      // Option not found check if it is a valid option
      // in the style of -Pprinter1 or ps -aux
      int p = 1;
      while (true)
      {
         QCString singleCharOption = " ";
         singleCharOption[0] = _opt[p];
         args = argsList->first();
         while (args)
         {
            enabled = _enabled;
            result = ::findOption(args->options, singleCharOption, opt_name, def, enabled);
            if (result) break;
            args = argsList->next();
         }
         if (!args)
            break; // Unknown argument

         p++;
         if (result == 1) // Single option
         {
            args->setOption(singleCharOption, enabled);
            if (_opt[p])
               continue; // Next option
            else
               return; // Finished
         }
         else if (result == 3) // This option takes an argument
         {
            if (argument.isEmpty())
            {
               argument = _opt+p;
            }
            args->setOption(singleCharOption, argument.data());
            return;
         }
         break; // Unknown argument
      }
      args = 0;
      result = 0;
   }

   if (!args || !result)
   {
      if (ignoreUnknown)
         return;
      enable_i18n();
      usage( i18n("Unknown option '%1'.").arg(QString::fromLocal8Bit(_opt)));
   }

   if ((result & 4) != 0)
   {
      result &= ~4;
      moreOptions = false;
   }

   if (result == 3) // This option takes an argument
   {
      if (!enabled)
      {
         if (ignoreUnknown)
            return;
         enable_i18n();
         usage( i18n("Unknown option '%1'.").arg(QString::fromLocal8Bit(_opt)));
      }
      if (argument.isEmpty())
      {
         i++;
         if (i >= argc)
         {
            enable_i18n();
            usage( i18n("'%1' missing.").arg( opt_name));
         }
         argument = argv[i];
      }
      args->setOption(opt, argument.data());
   }
   else
   {
      args->setOption(opt, enabled);
   }
}

void
KCmdLineArgs::printQ(const QString &msg)
{
#  if QT_VERSION<0x040000
   QCString localMsg = msg.local8Bit();
#else
   QCString localMsg = msg.toLocal8Bit();
#endif
   fprintf(stdout, "%s", localMsg.data());
}

void
KCmdLineArgs::parseAllArgs()
{
   bool allowArgs = false;
   bool inOptions = true;
   bool everythingAfterArgIsArgs = false;
   KCmdLineArgs *appOptions = argsList->last();
   if (!appOptions->id)
   {
     const KCmdLineOptions *option = appOptions->options;
     while(option && option->name)
     {
       if (option->name[0] == '+')
           allowArgs = true;
       if ( option->name[0] == '!' && option->name[1] == '+' )
       {
           allowArgs = true;
           everythingAfterArgIsArgs = true;
       }
       option++;
     }
   }
   for(int i = 1; i < argc; i++)
   {
      if (!argv[i])
         continue;

      if ((argv[i][0] == '-') && argv[i][1] && inOptions)
      {
         bool enabled = true;
         const char *option = &argv[i][1];
         const char *orig = argv[i];
         if (option[0] == '-')
         {
            option++;
            argv[i]++;
            if (!option[0])
            {
               inOptions = false;
               continue;
            }
         }
         if (::qstrcmp(option, "help") == 0)
         {
            usage(0);
         }
         else if (strncmp(option, "help-",5) == 0)
         {
            usage(option+5);
         }
         else if ( (::qstrcmp(option, "version") == 0) ||
                   (::qstrcmp(option, "v") == 0))
         {
            printQ( QString("Qt: %1\n").arg(qVersion()));
//            printQ( QString("KDE: %1\n").arg(KDE_VERSION_STRING));
            printQ( QString("%1: %2\n").
                        arg(about->programName()).arg(about->version()));
            exit(0);
         } else if ( (::qstrcmp(option, "license") == 0) )
         {
            enable_i18n();
            printQ( about->license() );
            printQ( "\n" );
            exit(0);
         } else if ( ::qstrcmp( option, "author") == 0 ) {
             enable_i18n();
             if ( about ) {
                 const QValueList<KAboutPerson> authors = about->authors();
                 if ( !authors.isEmpty() ) {
                     QString authorlist;
                     for (QValueList<KAboutPerson>::ConstIterator it = authors.begin(); it != authors.end(); ++it ) {
                         QString email;
                         if ( !(*it).emailAddress().isEmpty() )
                                 email = " <" + (*it).emailAddress() + ">";
                         authorlist += QString("    ") + (*it).name() + email + "\n";
                     }
                     printQ( i18n("the 2nd argument is a list of name+address, one on each line","%1 was written by\n%2").arg ( QString(about->programName()) ).arg( authorlist ) );
                 }
             } else {
                 printQ( i18n("%1 was written by somebody who wants to remain anonymous.").arg(about->programName()) );
             }
             if (!about->bugAddress().isEmpty())
             {
                if (about->bugAddress() == "submit@bugs.kde.org")
                    printQ( i18n( "Please use http://bugs.kde.org to report bugs, do not mail the authors directly.\n" ) );
                else
                    printQ( i18n( "Please use %1 to report bugs, do not mail the authors directly.\n" ).arg(about->bugAddress()) );
             }
             exit(0);
         } else {
           if ((option[0] == 'n') && (option[1] == 'o'))
           {
              option += 2;
              enabled = false;
           }
           findOption(orig, option, i, enabled, inOptions);
         }
      }
      else
      {
         // Check whether appOptions allows these arguments
         if (!allowArgs)
         {
            if (ignoreUnknown)
               continue;
            enable_i18n();
            usage( i18n("Unexpected argument '%1'.").arg(QString::fromLocal8Bit(argv[i])));
         }
         else
         {
            appOptions->addArgument(argv[i]);
            if (everythingAfterArgIsArgs)
                inOptions = false;
         }
      }
   }
   parsed = true;
}

/**
 * For KApplication only:
 *
 * Return argc
 */
int *
KCmdLineArgs::qt_argc()
{
//   if (!argsList)
//      KApplication::addCmdLineOptions(); // Lazy bastards!

   static int qt_argc = -1;
   if( qt_argc != -1 )
      return &qt_argc;

   KCmdLineArgs *args = parsedArgs("qt");
   assert(args); // No qt options have been added!
   if (!argv)
   {
      fprintf(stderr, "\n\nFAILURE (KCmdLineArgs):\n");
      fprintf(stderr, "Application has not called KCmdLineArgs::init(...).\n\n");

      assert( 0 );
      exit(255);
   }

   assert(argc >= (args->count()+1));
   qt_argc = args->count() +1;
   return &qt_argc;
}

/**
 * For KApplication only:
 *
 * Return argv
 */
char ***
KCmdLineArgs::qt_argv()
{
//   if (!argsList)
//      KApplication::addCmdLineOptions(); // Lazy bastards!

   static char** qt_argv;
   if( qt_argv != NULL )
      return &qt_argv;

   KCmdLineArgs *args = parsedArgs("qt");
   assert(args); // No qt options have been added!
   if (!argv)
   {
      fprintf(stderr, "\n\nFAILURE (KCmdLineArgs):\n");
      fprintf(stderr, "Application has not called KCmdLineArgs::init(...).\n\n");

      assert( 0 );
      exit(255);
   }

   qt_argv = new char*[ args->count() + 2 ];
   qt_argv[ 0 ] = qstrdup( appName());
   int i = 0;
   for(; i < args->count(); i++)
   {
      qt_argv[i+1] = qstrdup((char *) args->arg(i));
   }
   qt_argv[i+1] = 0;

   return &qt_argv;
}

void
KCmdLineArgs::enable_i18n()
{
    // called twice or too late
/*    if (KGlobal::_locale)
            return;

    if (!KGlobal::_instance) {
        KInstance *instance = new KInstance(about);
        (void) instance->config();
        // Don't delete instance!
    }*/
}

void
KCmdLineArgs::usage(const QString &error)
{
//    assert(KGlobal::_locale);
#  if QT_VERSION<0x040000
    QCString localError = error.local8Bit();
#else
    QCString localError = error.toLocal8Bit();
#endif
    if (localError[error.length()-1] == '\n')
        localError = localError.left(error.length()-1);
    fprintf(stderr, "%s: %s\n", argv[0], localError.data());

    QString tmp = i18n("Use --help to get a list of available command line options.");
#  if QT_VERSION<0x040000
    localError = tmp.local8Bit();
#else
    localError = tmp.toLocal8Bit();
#endif
    fprintf(stderr, "%s: %s\n", argv[0], localError.data());
    exit(254);
}

void
KCmdLineArgs::usage(const char *id)
{
   enable_i18n();
   assert(argsList != 0); // It's an error to call usage(...) without
                          // having done addCmdLineOptions first!

   QString optionFormatString           = "  %1 %2\n";
   QString optionFormatStringDef        = "  %1 %2 [%3]\n";
   QString optionHeaderString = i18n("\n%1:\n");
   QString tmp;
   QString usage;

   KCmdLineArgs *args = argsList->last();

   if (!(args->id) && (args->options) &&
       (args->options->name) && (args->options->name[0] != '+'))
   {
      usage = i18n("[options] ")+usage;
   }

   while(args)
   {
      if (args->name)
      {
         usage = i18n("[%1-options]").arg(args->name)+" "+usage;
      }
      args = argsList->prev();
   }

   KCmdLineArgs *appOptions = argsList->last();
   if (!appOptions->id)
   {
     const KCmdLineOptions *option = appOptions->options;
     while(option && option->name)
     {
       if (option->name[0] == '+')
          usage = usage + (option->name+1) + " ";
       else if ( option->name[0] == '!' && option->name[1] == '+' )
          usage = usage + (option->name+2) + " ";

       option++;
     }
   }

   printQ(i18n("Usage: %1 %2\n").arg(argv[0]).arg(usage));
   printQ("\n"+about->shortDescription()+"\n");

   printQ(optionHeaderString.arg(i18n("Generic options")));
   printQ(optionFormatString.arg("--help", -25).arg(i18n("Show help about options")));

   args = argsList->first();
   while(args)
   {
      if (args->name && args->id)
      {
         QString option = QString("--help-%1").arg(args->id);
         QString desc = i18n("Show %1 specific options").arg(args->name);

         printQ(optionFormatString.arg(option, -25).arg(desc));
      }
      args = argsList->next();
   }

   printQ(optionFormatString.arg("--help-all",-25).arg(i18n("Show all options")));
   printQ(optionFormatString.arg("--author",-25).arg(i18n("Show author information")));
   printQ(optionFormatString.arg("-v, --version",-25).arg(i18n("Show version information")));
   printQ(optionFormatString.arg("--license",-25).arg(i18n("Show license information")));
   printQ(optionFormatString.arg("--", -25).arg(i18n("End of options")));

   args = argsList->first(); // Sets current to 1st.

   bool showAll = id && (::qstrcmp(id, "all") == 0);

   if (!showAll)
   {
     while(args)
     {
       if (!id && !args->id) break;
       if (id && (::qstrcmp(args->id, id) == 0)) break;
       args = argsList->next();
     }
   }

   while(args)
   {
     bool hasArgs = false;
     bool hasOptions = false;
     QString optionsHeader;
     if (args->name)
        optionsHeader = optionHeaderString.arg(i18n("%1 options").arg(QString::fromLatin1(args->name)));
     else
        optionsHeader = i18n("\nOptions:\n");

     while (args)
     {
       const KCmdLineOptions *option = args->options;
       QCString opt = "";
//
       while(option && option->name)
       {
         QString description;
         QString descriptionRest;
         QStringList dl;

         // Option header
         if (option->name[0] == ':')
         {
            if (option->description)
            {
               optionsHeader = "\n"+i18n(option->description);
               if (!optionsHeader.endsWith("\n"))
                  optionsHeader.append("\n");
               hasOptions = false;
            }
            option++;
            continue;
         }

         // Free-form comment
         if (option->name[0] == 0)
         {
            if (option->description)
            {
               QString tmp = "\n"+i18n(option->description);
               if (!tmp.endsWith("\n"))
                  tmp.append("\n");
               printQ(tmp);
            }
            option++;
            continue;
         }

         // Options
         if (option->description)
         {
            description = i18n(option->description);
#  if QT_VERSION<0x040000
            dl = QStringList::split("\n", description, true);
#else
            dl = description.split("\n");
#endif
            description = dl.first();
            dl.erase( dl.begin() );
         }
         QCString name = option->name;
         if (name[0] == '!')
             name = name.mid(1);

         if (name[0] == '+')
         {
            if (!hasArgs)
            {
               printQ(i18n("\nArguments:\n"));
               hasArgs = true;
            }

            name = name.mid(1);
            if ((name[0] == '[') && (name[name.length()-1] == ']'))
               name = name.mid(1, name.length()-2);
            printQ(optionFormatString.arg(QString(name), -25)
                 .arg(description));
         }
         else
         {
            if (!hasOptions)
            {
               printQ(optionsHeader);
               hasOptions = true;
            }

            if ((name.length() == 1) || (name[1] == ' '))
               name = "-"+name;
            else
               name = "--"+name;
            if (!option->description)
            {
               opt = name + ", ";
            }
            else
            {
               opt = opt + name;
               if (!option->def)
               {
                  printQ(optionFormatString.arg(QString(opt), -25)
                         .arg(description));
               }
               else
               {
                  printQ(optionFormatStringDef.arg(QString(opt), -25)
                         .arg(description).arg(option->def));
               }
               opt = "";
            }
         }
         for(QStringList::Iterator it = dl.begin();
             it != dl.end();
             ++it)
         {
            printQ(optionFormatString.arg("", -25).arg(*it));
         }

         option++;
       }
       args = argsList->next();
       if (!args || args->name || !args->id) break;
     }
     if (!showAll) break;
   }

   exit(254);
}

//
// Member functions
//

/**
 *  Constructor.
 *
 *  The given arguments are assumed to be constants.
 */
KCmdLineArgs::KCmdLineArgs( const KCmdLineOptions *_options,
                            const char *_name, const char *_id)
  : options(_options), name(_name), id(_id)
{
  parsedOptionList = 0;
  parsedArgList = 0;
  isQt = (::qstrcmp(id, "qt") == 0);
}

/**
 *  Destructor.
 */
KCmdLineArgs::~KCmdLineArgs()
{
  delete parsedOptionList;
  delete parsedArgList;
  if (argsList)
     argsList->removeRef(this);
}

void
KCmdLineArgs::clear()
{
   delete parsedArgList;
   parsedArgList = 0;
   delete parsedOptionList;
   parsedOptionList = 0;
}

void
KCmdLineArgs::reset()
{
   if ( argsList ) {
      argsList->setAutoDelete( true );
      argsList->clear();
      delete argsList;
      argsList = 0;
   }
   parsed = false;
}

void
KCmdLineArgs::save( QDataStream &ds) const
{
   uint count = 0;
   if (parsedOptionList)
      parsedOptionList->save( ds );
   else
      ds << count;

   if (parsedArgList)
      parsedArgList->save( ds );
   else
      ds << count;
}

void
KCmdLineArgs::load( QDataStream &ds)
{
   if (!parsedOptionList) parsedOptionList = new KCmdLineParsedOptions;
   if (!parsedArgList) parsedArgList = new KCmdLineParsedArgs;

   parsedOptionList->load( ds );
   parsedArgList->load( ds );

   if (parsedOptionList->count() == 0)
   {
      delete parsedOptionList;
      parsedOptionList = 0;
   }
   if (parsedArgList->count() == 0)
   {
      delete parsedArgList;
      parsedArgList = 0;
   }
}

void
KCmdLineArgs::setOption(const QCString &opt, bool enabled)
{
   if (isQt)
   {
      // Qt does it own parsing.
      QCString arg = "-";
      if( !enabled )
          arg += "no";
      arg += opt;
      addArgument(arg);
   }
   if (!parsedOptionList) {
        parsedOptionList = new KCmdLineParsedOptions;
        parsedOptionList->setAutoDelete(true);
   }

   if (enabled)
      parsedOptionList->replace( opt, new QCString("t") );
   else
      parsedOptionList->replace( opt, new QCString("f") );
}

void
KCmdLineArgs::setOption(const QCString &opt, const char *value)
{
   if (isQt)
   {
      // Qt does it's own parsing.
      QCString arg = "-";
      arg += opt;
      addArgument(arg);
      addArgument(value);

#ifdef Q_WS_X11
      // Hack coming up!
      if (arg == "-display")
      {
         setenv(DISPLAY, value, true);
      }
#endif
   }
   if (!parsedOptionList) {
        parsedOptionList = new KCmdLineParsedOptions;
        parsedOptionList->setAutoDelete(true);
   }

   parsedOptionList->insert( opt, new QCString(value) );
}

QCString
KCmdLineArgs::getOption(const char *_opt) const
{
   QCString *value = 0;
   if (parsedOptionList)
   {
      value = parsedOptionList->find(_opt);
   }

   if (value)
      return (*value);

   // Look up the default.
   const char *opt_name;
   const char *def;
   bool dummy = true;
   QCString opt = _opt;
   int result = ::findOption( options, opt, opt_name, def, dummy) & ~4;

   if (result != 3)
   {
      fprintf(stderr, "\n\nFAILURE (KCmdLineArgs):\n");
      fprintf(stderr, "Application requests for getOption(\"%s\") but the \"%s\" option\n",
                      _opt, _opt);
      fprintf(stderr, "has never been specified via addCmdLineOptions( ... )\n\n");

      assert( 0 );
      exit(255);
   }
   return QCString(def);
}

QCStringList
KCmdLineArgs::getOptionList(const char *_opt) const
{
   QCStringList result;
   if (!parsedOptionList)
      return result;

   while(true)
   {
      QCString *value = parsedOptionList->take(_opt);
      if (!value)
         break;
      result.prepend(*value);
      delete value;
   }

   // Reinsert items in dictionary
   // WABA: This is rather silly, but I don't want to add restrictions
   // to the API like "you can only call this function once".
   // I can't access all items without taking them out of the list.
   // So taking them out and then putting them back is the only way.
   for(QCStringList::ConstIterator it=result.begin();
       it != result.end();
       ++it)
   {
      parsedOptionList->insert(_opt, new QCString(*it));
   }
   return result;
}

bool
KCmdLineArgs::isSet(const char *_opt) const
{
   // Look up the default.
   const char *opt_name;
   const char *def;
   bool dummy = true;
   QCString opt = _opt;
   int result = ::findOption( options, opt, opt_name, def, dummy) & ~4;

   if (result == 0)
   {
      fprintf(stderr, "\n\nFAILURE (KCmdLineArgs):\n");
      fprintf(stderr, "Application requests for isSet(\"%s\") but the \"%s\" option\n",
                      _opt, _opt);
      fprintf(stderr, "has never been specified via addCmdLineOptions( ... )\n\n");

      assert( 0 );
      exit(255);
   }

   QCString *value = 0;
   if (parsedOptionList)
   {
      value = parsedOptionList->find(opt);
   }

   if (value)
   {
      if (result == 3)
         return true;
      else
         return ((*value)[0] == 't');
   }

   if (result == 3)
      return false; // String option has 'false' as default.

   // We return 'true' as default if the option was listed as '-nofork'
   // We return 'false' as default if the option was listed as '-fork'
   return (result == 2);
}

int
KCmdLineArgs::count() const
{
   if (!parsedArgList)
      return 0;
   return parsedArgList->count();
}

const char *
KCmdLineArgs::arg(int n) const
{
   if (!parsedArgList || (n >= (int) parsedArgList->count()))
   {
      fprintf(stderr, "\n\nFAILURE (KCmdLineArgs): Argument out of bounds\n");
      fprintf(stderr, "Application requests for arg(%d) without checking count() first.\n",
                      n);

      assert( 0 );
      exit(255);
   }

   return parsedArgList->at(n);
}

KURL
KCmdLineArgs::url(int n) const
{
   return makeURL( arg(n) );
}

KURL KCmdLineArgs::makeURL(const char *_urlArg)
{
   QString urlArg = QFile::decodeName(_urlArg);
   if (!QDir::isRelativePath(urlArg))
   {
      //KURL result;
      //result.setPath(urlArg);
      //return result; // Absolute path.
      return urlArg;
   }
#  if QT_VERSION<0x040000
   if ( !QUrl::isRelativeUrl(urlArg) )
#else
   if ( !QUrl(urlArg).isRelative() )
#endif
     //return KURL(urlArg); // Argument is a URL
     return urlArg;

//   KURL result;
//   result.setPath( cwd()+"/"+urlArg );
//   result.cleanPath();
//   return result;  // Relative path
    return cwd() + "/" + urlArg;
}

void
KCmdLineArgs::addArgument(const char *argument)
{
   if (!parsedArgList)
      parsedArgList = new KCmdLineParsedArgs;

   parsedArgList->append(argument);
}

static const KCmdLineOptions kde_tempfile_option[] =
{
   { "tempfile",       I18N_NOOP("The files/URLs opened by the application will be deleted after use"), 0},
   KCmdLineLastOption
};

void
KCmdLineArgs::addTempFileOption()
{
    KCmdLineArgs::addCmdLineOptions( kde_tempfile_option, "KDE-tempfile", "kde-tempfile" );
}

bool KCmdLineArgs::isTempFileSet()
{
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs( "kde-tempfile" );
    if ( args )
        return args->isSet( "tempfile" );
    return false;
}
