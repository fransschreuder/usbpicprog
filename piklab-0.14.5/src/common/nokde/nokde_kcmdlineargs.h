// modified from KDE 3.4 for Windows port (Nicolas Hadacek)

/* This file is part of the KDE project
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

#ifndef _KCMDLINEARGS_H_
#define _KCMDLINEARGS_H_

#include "nokde_klocale.h"
#include "nokde_kurl.h"

#  if QT_VERSION<0x040000
#  include <qptrlist.h>
#  include <qvaluelist.h>
#else
#  include <Qt3Support/Q3CString>
#  define QCString Q3CString
#  include <Qt3Support/Q3PtrList>
#  define QPtrList Q3PtrList
#  include <Qt3Support/Q3ValueList>
#  define QValueList Q3ValueList
#endif
#  include <qstring.h>

typedef QValueList<QCString> QCStringList;

/**
 * @short Structure that holds command line options.
 *
 * This class is intended to be used with the KCmdLineArgs class, which
 * provides convenient and powerful command line argument parsing and
 * handling functionality.
 *
 * @see KCmdLineArgs for additional usage information
 */
struct KDECORE_EXPORT KCmdLineOptions
{
   /**
    * The name of the argument as it should be called on the command line and
    * appear in <i>myapp --help</i>.
    *
    * Note that if this option starts with "no" that you will need to test for
    * the name without the "no" and the result will be the inverse of what is
    * specified. i.e. if "nofoo" is the name of the option and
    * <i>myapp --nofoo</i> is called:
    *
    * \code
    * KCmdLineArgs::parsedArgs()->isSet("foo"); // false
    * \endcode
    */
   const char *name;
   /**
    * The text description of the option as should appear in
    * <i>myapp --help</i>.  This value should be wrapped with I18N_NOOP().
    */
   const char *description;
   /**
    * The default value for the option, if it is not specified on the
    * command line.
    */
   const char *def; // Default
};

#define KCmdLineLastOption { 0, 0, 0 }

class KCmdLineArgsList;
class KApplication;
class KUniqueApplication;
class KCmdLineParsedOptions;
class KCmdLineParsedArgs;
class KAboutData;
class KCmdLineArgsPrivate;

/**
 *  @short A class for command-line argument handling.
 *
 *  KCmdLineArgs provides simple access to the command-line arguments
 *  for an application. It takes into account Qt-specific options,
 *  KDE-specific options and application specific options.
 *
 *  This class is used in %main() via the static method
 *  init().
 *
 *  A typical %KDE application using %KCmdLineArgs should look like this:
 *
 *  \code
 *  int main(int argc, char *argv[])
 *  {
 *     // Initialize command line args
 *     KCmdLineArgs::init(argc, argv, appName, programName, description, version);
 *
 *     // Tell which options are supported
 *     KCmdLineArgs::addCmdLineOptions( options );
 *
 *     // Add options from other components
 *     KUniqueApplication::addCmdLineOptions();
 *
 *     ....
 *
 *     // Create application object without passing 'argc' and 'argv' again.
 *     KUniqueApplication app;
 *
 *     ....
 *
 *     // Handle our own options/arguments
 *     // A KApplication will usually do this in main but this is not
 *     // necessary.
 *     // A KUniqueApplication might want to handle it in newInstance().
 *
 *     KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
 *
 *     // A binary option (on / off)
 *     if (args->isSet("some-option"))
 *        ....
 *
 *     // An option which takes an additional argument
 *     QCString anotherOptionArg = args->getOption("another-option");
 *
 *     // Arguments (e.g. files to open)
 *     for(int i = 0; i < args->count(); i++) // Counting start at 0!
 *     {
 *        // don't forget to convert to Unicode!
 *        openFile( QFile::decodeName( args->arg(i)));
 *        // Or more convenient:
 *        // openURL( args->url(i));
 *
 *     }
 *
 *     args->clear(); // Free up some memory.
 *     ....
 *  }
 *  \endcode
 *
 *  The options that an application supports are configured using the
 *  KCmdLineOptions class. An example is shown below:
 *
 *  \code
 *  static const KCmdLineOptions options[] =
 *  {
 *     { "a", I18N_NOOP("A short binary option"), 0 },
 *     { "b \<file>", I18N_NOOP("A short option which takes an argument"), 0 },
 *     { "c \<speed>", I18N_NOOP("As above but with a default value"), "9600" },
 *     { "option1", I18N_NOOP("A long binary option, off by default"), 0 },
 *     { "nooption2", I18N_NOOP("A long binary option, on by default"), 0 },
 *     { ":", I18N_NOOP("Extra options:"), 0 },
 *     { "option3 \<file>", I18N_NOOP("A long option which takes an argument"), 0 },
 *     { "option4 \<speed>", I18N_NOOP("A long option which takes an argument, defaulting to 9600"), "9600" },
 *     { "d", 0, 0 },
 *     { "option5", I18N_NOOP("A long option which has a short option as alias"), 0 },
 *     { "e", 0, 0 },
 *     { "nooption6", I18N_NOOP("Another long option with an alias"), 0 },
 *     { "f", 0, 0 },
 *     { "option7 \<speed>", I18N_NOOP("'--option7 speed' is the same as '-f speed'"), 0 },
 *     { "!option8 \<cmd>", I18N_NOOP("All options following this one will be treated as arguments"), 0 },
 *     { "+file", I18N_NOOP("A required argument 'file'"), 0 },
 *     { "+[arg1]", I18N_NOOP("An optional argument 'arg1'"), 0 },
 *     { "!+command", I18N_NOOP("A required argument 'command', that can contain multiple words, even starting with '-'"), 0 },
 *     { "", I18N_NOOP("Additional help text not associated with any particular option") 0 },
 *     KCmdLineLastOption // End of options.
 *  };
 *  \endcode
 *
 *  The I18N_NOOP macro is used to indicate that these strings should be
 *  marked for translation. The actual translation is done by KCmdLineArgs.
 *  You can't use i18n() here because we are setting up a static data
 *  structure and can't do translations at compile time.
 *
 *  Note that a program should define the options before any arguments.
 *
 *  When a long option has a short option as an alias, a program should
 *  only test for the long option.
 *
 *  With the above options a command line could look like:
 *  \code
 *     myapp -a -c 4800 --display localhost:0.0 --nooption5 -d /tmp/file
 *  \endcode
 *
 *  Long binary options can be in the form 'option' and 'nooption'.
 *  A command line may contain the same binary option multiple times,
 *  the last option determines the outcome:
 *  \code
 *     myapp --nooption4 --option4 --nooption4
 *  \endcode
 *  is the same as:
 *  \code
 *     myapp --nooption4
 *  \endcode
 *
 *  If an option value is provided multiple times, normally only the last
 *  value is used:
 *  \code
 *     myapp -c 1200 -c 2400 -c 4800
 *  \endcode
 *  is usually the same as:
 *  \code
 *     myapp -c 4800
 *  \endcode
 *
 *  However, an application can choose to use all values specified as well.
 *  As an example of this, consider that you may wish to specify a
 *  number of directories to use:
 *  \code
 *     myapp -I /usr/include -I /opt/kde/include -I /usr/X11/include
 *  \endcode
 *  When an application does this it should mention this in the description
 *  of the option. To access these options, use getOptionList()
 *
 *  Tips for end-users:
 *
 *  @li Single char options like "-a -b -c" may be combined into "-abc"
 *  @li The option "--foo bar" may also be written "--foo=bar"
 *  @li The option "-P lp1" may also be written "-P=lp1" or "-Plp1"
 *  @li The option "--foo bar" may also be written "-foo bar"
 *
 *  @author Waldo Bastian
 *  @version 0.0.4
 */
class KDECORE_EXPORT KCmdLineArgs
{
  friend class KApplication;
  friend class KUniqueApplication;
  friend class QPtrList<KCmdLineArgs>;
public:
  // Static functions:

  /**
   * Initialize class.
   *
   * This function should be called as the very first thing in
   *  your application.
   * @param _argc As passed to @p main(...).
   * @param _argv As passed to @p main(...).
   * @param _appname The untranslated name of your application. This should
   *                match with @p argv[0].
   * @param programName A program name string to be used for display
   *        purposes. This string should be marked for
   *        translation. Example: I18N_NOOP("KEdit")
   * @param _description A short description of what your application is about.
   * @param _version A version.
   * @param noKApp Set this true to not add commandline options for
   *        QApplication / KApplication
   *
   * @since 3.2
   */
   static void init(int _argc, char **_argv, const char *_appname,
                    const char* programName, const char *_description,
                    const char *_version, bool noKApp = false);
   /**
    * @deprecated
    * You should convert any calls to this method to use the one
    * above, by adding in the program name to be used for display
    * purposes. Do not forget to mark it for translation using I18N_NOOP.
    */
  static void init(int _argc, char **_argv,
                   const char *_appname, const char *_description,
                   const char *_version, bool noKApp = false) KDE_DEPRECATED;

  /**
   * Initialize class.
   *
   * This function should be called as the very first thing in
   *  your application. It uses KAboutData to replace some of the
   *  arguments that would otherwise be required.
   *
   * @param _argc As passed to @p main(...).
   * @param _argv As passed to @p main(...).
   * @param about A KAboutData object describing your program.
   * @param noKApp Set this true to not add commandline options for
   *        QApplication / KApplication
   */
  static void init(int _argc, char **_argv,
                   const KAboutData *about, bool noKApp = false);

  /**
   * Initialize Class
   *
   * This function should be called as the very first thing in your
   * application. This method will rarely be used, since it doesn't
   * provide any argument parsing. It does provide access to the
   * KAboutData information.
   * This method is exactly the same as calling
   * init(0,0, const KAboutData *about, true).
   *
   * @param about the about data.
   * \see KAboutData
   */
  static void init(const KAboutData *about);

  /**
   * Add options to your application.
   *
   * You must make sure that all possible options have been added before
   * any class uses the command line arguments.
   *
   * The list of options should look like this:
   *
   * \code
   * static KCmdLineOptions options[] =
   * {
   *    { "option1 \<argument>", I18N_NOOP("Description 1"), "my_extra_arg" },
   *    { "o", 0, 0 },
   *    { "option2", I18N_NOOP("Description 2"), 0 },
   *    { "nooption3", I18N_NOOP("Description 3"), 0 },
   *    KCmdLineLastOption
   * }
   * \endcode
   *
   * @li "option1" is an option that requires an additional argument,
   *     but if one is not provided, it uses "my_extra_arg".
   * @li "option2" is an option that can be turned on. The default is off.
   * @li "option3" is an option that can be turned off. The default is on.
   * @li "o" does not have a description. It is an alias for the option
   *     that follows. In this case "option2".
   * @li "+file" specifies an argument. The '+' is removed. If your program
   *     doesn't specify that it can use arguments your program will abort
   *     when an argument is passed to it. Note that the reverse is not
   *     true. If required, you must check yourself the number of arguments
   *     specified by the user:
   *     \code
   *       KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
   *       if (args->count() == 0) KCmdLineArgs::usage(i18n("No file specified!"));
   *     \endcode
   *
   * In BNF:
   * \code
   * cmd = myapp [options] file
   * options = (option)*
   * option = --option1 \<argument> |
   *          (-o | --option2 | --nooption2) |
   *          ( --option3 | --nooption3 )
   * \endcode
   *
   * Instead of "--option3" one may also use "-option3"
   *
   * Usage examples:
   *
   * @li "myapp --option1 test"
   * @li "myapp" (same as "myapp --option1 my_extra_arg")
   * @li "myapp --option2"
   * @li "myapp --nooption2" (same as "myapp", since it is off by default)
   * @li "myapp -o" (same as "myapp --option2")
   * @li "myapp --nooption3"
   * @li "myapp --option3 (same as "myapp", since it is on by default)
   * @li "myapp --option2 --nooption2" (same as "myapp", because it
   *     option2 is off by default, and the last usage applies)
   * @li "myapp /tmp/file"
   *
   * @param options A list of options that your code supplies.
   * @param name the name of the option, can be 0.
   * @param id A name with which these options can be identified, can be 0.
   * @param afterId The options are inserted after this set of options, can be 0.
   */
  static void addCmdLineOptions( const KCmdLineOptions *options,
                                 const char *name=0, const char *id = 0,
                                 const char *afterId=0);

  /**
   * Access parsed arguments.
   *
   * This function returns all command line arguments that your code
   * handles. If unknown command-line arguments are encountered the program
   * is aborted and usage information is shown.
   *
   * @param id The name of the options you are interested in, can be 0.
   */
  static KCmdLineArgs *parsedArgs(const char *id=0);

  /**
   * Get the CWD (Current Working Directory) associated with the
   * current command line arguments.
   *
   * Typically this is needed in KUniqueApplication::newInstance()
   * since the CWD of the process may be different from the CWD
   * where the user started a second instance.
   * @return the current working directory
   **/
  static QString cwd();

  /**
   * Get the appname according to argv[0].
   * @return the name of the application
   **/
  static const char *appName();

  /**
   * Print the usage help to stdout and exit.
   *
   * @param id if 0, print all options. If id is set, only print the
   *        option specified by id. The id is the value set by
   *        #ref addCmdLineOptions().
   **/
  static void usage(const char *id = 0);

  /**
   * Print an error to stderr and the usage help to stdout and exit.
   * @param error the error to print
   **/
  static void usage(const QString &error);

  /**
   * Enable i18n to be able to print a translated error message.
   *
   * N.B.: This function leaks memory, therefore you are expected to exit
   * afterwards (e.g., by calling usage()).
   **/
  static void enable_i18n();

  // Member functions:


  /**
   *  Read out a string option.
   *
   *  The option must have a corresponding KCmdLineOptions entry
   *  of the form:
   *  \code
   *    { "option \<argument>", I18N_NOOP("Description"), "default" }
   *  \endcode
   *  You cannot test for the presence of an alias - you must always
   *  test for the full option.
   *
   *  @param option The name of the option without '-'.
   *
   *  @return The value of the option. If the option was not
   *          present on the command line the default is returned.
   *          If the option was present more than the value of the
   *          last occurrence is used.
   */
  QCString getOption(const char *option) const;

  /**
   *  Read out all occurrences of a string option.
   *
   *  The option must have a corresponding KCmdLineOptions entry
   *  of the form:
   *  \code
   *    { "option \<argument>", I18N_NOOP("Description"), "default" }
   *  \endcode
   *  You cannot test for the presence of an alias - you must always
   *  test for the full option.
   *
   *  @param option The name of the option, without '-' or '-no'.
   *
   *  @return A list of all option values. If no option was present
   *          on the command line, an empty list is returned.
   */
  QCStringList getOptionList(const char *option) const;

  /**
   *  Read out a boolean option or check for the presence of string option.
   *
   *  @param option The name of the option without '-' or '-no'.
   *
   *  @return The value of the option. It will be true if the option
   *  was specifically turned on in the command line, or if the option
   *  is turned on by default (in the KCmdLineOptions list) and was
   *  not specifically turned off in the command line. Equivalently,
   *  it will be false if the option was specifically turned off in
   *  the command line, or if the option is turned off by default (in
   *  the KCmdLineOptions list) and was not specifically turned on in
   *  the command line.
   */
  bool isSet(const char *option) const;

  /**
   *  Read the number of arguments that aren't options (but,
   *  for example, filenames).
   *
   *  @return The number of arguments that aren't options
   */
  int count() const;

  /**
   *  Read out an argument.
   *
   *  @param n The argument to read. 0 is the first argument.
   * count()-1 is the last argument.
   *
   *  @return A @p const @p char @p * pointer to the n'th argument.
   */
  const char *arg(int n) const;

  /**
   *  Read out an argument representing a URL.
   *
   *  The argument can be
   *  @li an absolute filename
   *  @li a relative filename
   *  @li a URL
   *
   *  @param n The argument to read. 0 is the first argument.
   * count()-1 is the last argument.
   *
   *  @return a URL representing the n'th argument.
   */
  KURL url(int n) const;

  /**
   * Used by url().
   * Made public for apps that don't use KCmdLineArgs
   * @param urlArg the argument
   * @return the url.
   */
  static KURL makeURL( const char * urlArg );

  /**
   * Made public for apps that don't use KCmdLineArgs
   * To be done before makeURL, to set the current working
   * directory in case makeURL needs it.
   * @param cwd the new working directory
   */
  static void setCwd( char * cwd ) { mCwd = cwd; }

  /**
   *  Clear all options and arguments.
   */
  void clear();

  /**
   *  Reset all option definitions, i.e. cancel all addCmdLineOptions calls.
   *  Note that KApplication's options are removed too, you might want to
   *  call KApplication::addCmdLineOptions if you want them back.
   *
   *  You usually don't want to call this method.
   */
  static void reset();

  /**
   * Load arguments from a stream.
   */
  static void loadAppArgs( QDataStream &);

  /**
   * Add standard option --tempfile
   * @since 3.4
   */
  static void addTempFileOption();

  // this avoids having to know the "id" used by addTempFileOption
  // but this approach doesn't scale well, we can't have 50 standard options here...
  /**
   * @return true if --tempfile was set
   * @since 3.4
   */
  static bool isTempFileSet();

protected:
  /**
   * @internal
   *  Constructor.
   */
  KCmdLineArgs( const KCmdLineOptions *_options, const char *_name,
                const char *_id);

  /**
   *  @internal use only.
   *
   *  Use clear() if you want to free up some memory.
   *
   *  Destructor.
   */
  ~KCmdLineArgs();

private:
  /**
   * @internal
   *
   * Checks what to do with a single option
   */
  static void findOption(const char *_opt, QCString opt, int &i, bool enabled, bool &moreOptions);

  /**
   * @internal
   *
   * Parse all arguments, verify correct syntax and put all arguments
   * where they belong.
   */
  static void parseAllArgs();

  /**
   * @internal for KApplication only:
   *
   * Return argc
   */
  static int *qt_argc();

  /**
   * @internal for KApplication only:
   *
   * Return argv
   */

  static char ***qt_argv();

  /**
   * @internal
   *
   * Remove named options.
   *
   * @param id The name of the options to be removed.
   */
  static void removeArgs(const char *id);

  /**
   * @internal for KUniqueApplication only:
   *
   * Save all but the Qt and KDE arguments to a stream.
   */
  static void saveAppArgs( QDataStream &);

  /**
   * @internal
   *
   *  Set a boolean option
   */
  void setOption(const QCString &option, bool enabled);

  /**
   * @internal
   *
   *  Set a string option
   */
  void setOption(const QCString &option, const char *value);

  /**
   * @internal
   *
   * Add an argument
   */
  void addArgument(const char *argument);

  /**
   * @internal
   *
   * Save to a stream.
   */
  void save( QDataStream &) const;

  /**
   * @internal
   *
   * Restore from a stream.
   */
  void load( QDataStream &);

  /**
   * @internal for KApplication only
   *
   * Initialize class.
   *
   * This function should be called as the very first thing in
   *  your application.
   * @param argc As passed to @p main(...).
   * @param argv As passed to @p main(...).
   * @param appname The untranslated name of your application. This should
   *                match with @p argv[0].
   *
   * This function makes KCmdLineArgs ignore all unknown options as well as
   * all arguments.
   */
  static void initIgnore(int _argc, char **_argv, const char *_appname);

  static void printQ(const QString &msg);

  const KCmdLineOptions *options;
  const char *name;
  const char *id;
  KCmdLineParsedOptions *parsedOptionList;
  KCmdLineParsedArgs *parsedArgList;
  bool isQt;

  static KCmdLineArgsList *argsList; // All options.
  static const KAboutData *about;

  static int argc; // The original argc
  static char **argv; // The original argv
  static bool parsed; // Whether we have parsed the arguments since calling init
  static bool ignoreUnknown; // Ignore unknown options and arguments
  static char *mCwd; // Current working directory. Important for KUnqiueApp!
  static bool parseArgs;

  KCmdLineArgsPrivate *d;
};

#endif

