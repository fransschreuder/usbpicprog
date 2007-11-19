// modified from KDE 3.4 for Windows port (Nicolas Hadacek)

/*
 * This file is part of the KDE Libraries
 * Copyright (C) 2000 Espen Sand (espen@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */


#include "nokde_kaboutdata.h"
//#include <kstandarddirs.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>

QString
KAboutPerson::name() const
{
   return QString::fromUtf8(mName);
}

QString
KAboutPerson::task() const
{
   if (mTask && *mTask)
      return i18n(mTask);
   else
      return QString::null;
}

QString
KAboutPerson::emailAddress() const
{
   return QString::fromUtf8(mEmailAddress);
}


QString
KAboutPerson::webAddress() const
{
   return QString::fromUtf8(mWebAddress);
}


KAboutTranslator::KAboutTranslator(const QString & name,
                const QString & emailAddress)
{
    mName=name;
    mEmail=emailAddress;
}

QString KAboutTranslator::name() const
{
    return mName;
}

QString KAboutTranslator::emailAddress() const
{
    return mEmail;
}

class KAboutDataPrivate
{
public:
    KAboutDataPrivate()
        : translatorName("_: NAME OF TRANSLATORS\nYour names")
        , translatorEmail("_: EMAIL OF TRANSLATORS\nYour emails")
        , productName(0)
//        , programLogo(0)
        {}
    ~KAboutDataPrivate()
        {
//             delete programLogo;
        }
    const char *translatorName;
    const char *translatorEmail;
    const char *productName;
//    QImage* programLogo;
};



KAboutData::KAboutData( const char *appName,
                        const char *programName,
                        const char *version,
                        const char *shortDescription,
                        int licenseType,
                        const char *copyrightStatement,
                        const char *text,
                        const char *homePageAddress,
                        const char *bugsEmailAddress
                        ) :
  mProgramName( programName ),
  mVersion( version ),
  mShortDescription( shortDescription ),
  mLicenseKey( licenseType ),
  mCopyrightStatement( copyrightStatement ),
  mOtherText( text ),
  mHomepageAddress( homePageAddress ),
  mBugEmailAddress( bugsEmailAddress ),
  mLicenseText (0)
{
   d = new KAboutDataPrivate;

   if( appName ) {
     const char *p = strrchr(appName, '/');
     if( p )
         mAppName = p+1;
     else
         mAppName = appName;
   } else
     mAppName = 0;
}

KAboutData::~KAboutData()
{
    if (mLicenseKey == License_File)
        delete [] mLicenseText;
    delete d;
}

void
KAboutData::addAuthor( const char *name, const char *task,
                    const char *emailAddress, const char *webAddress )
{
  mAuthorList.append(KAboutPerson(name,task,emailAddress,webAddress));
}

void
KAboutData::addCredit( const char *name, const char *task,
                    const char *emailAddress, const char *webAddress )
{
  mCreditList.append(KAboutPerson(name,task,emailAddress,webAddress));
}

void
KAboutData::setTranslator( const char *name, const char *emailAddress)
{
  d->translatorName=name;
  d->translatorEmail=emailAddress;
}

void
KAboutData::setLicenseText( const char *licenseText )
{
  mLicenseText = licenseText;
  mLicenseKey = License_Custom;
}

void
KAboutData::setLicenseTextFile( const QString &file )
{
  mLicenseText = qstrdup(QFile::encodeName(file));
  mLicenseKey = License_File;
}

void
KAboutData::setAppName( const char *appName )
{
  mAppName = appName;
}

void
KAboutData::setProgramName( const char* programName )
{
  mProgramName = programName;
}

void
KAboutData::setVersion( const char* version )
{
  mVersion = version;
}

void
KAboutData::setShortDescription( const char *shortDescription )
{
  mShortDescription = shortDescription;
}

void
KAboutData::setLicense( LicenseKey licenseKey)
{
  mLicenseKey = licenseKey;
}

void
KAboutData::setCopyrightStatement( const char *copyrightStatement )
{
  mCopyrightStatement = copyrightStatement;
}

void
KAboutData::setOtherText( const char *otherText )
{
  mOtherText = otherText;
}

void
KAboutData::setHomepage( const char *homepage )
{
  mHomepageAddress = homepage;
}

void
KAboutData::setBugAddress( const char *bugAddress )
{
  mBugEmailAddress = bugAddress;
}

void
KAboutData::setProductName( const char *productName )
{
  d->productName = productName;
}

const char *
KAboutData::appName() const
{
   return mAppName;
}

const char *
KAboutData::productName() const
{
   if (d->productName)
      return d->productName;
   else
      return appName();
}

QString
KAboutData::programName() const
{
   if (mProgramName && *mProgramName)
      return i18n(mProgramName);
   else
      return QString::null;
}
/*
QImage
KAboutData::programLogo() const
{
    return d->programLogo ? (*d->programLogo) : QImage();
}

void
KAboutData::setProgramLogo(const QImage& image)
{
    if (!d->programLogo)
       d->programLogo = new QImage( image );
    else
       *d->programLogo = image;
}
*/
QString
KAboutData::version() const
{
   return QString::fromLatin1(mVersion);
}

QString
KAboutData::shortDescription() const
{
   if (mShortDescription && *mShortDescription)
      return i18n(mShortDescription);
   else
      return QString::null;
}

QString
KAboutData::homepage() const
{
   return QString::fromLatin1(mHomepageAddress);
}

QString
KAboutData::bugAddress() const
{
   return QString::fromLatin1(mBugEmailAddress);
}

const QValueList<KAboutPerson>
KAboutData::authors() const
{
   return mAuthorList;
}

const QValueList<KAboutPerson>
KAboutData::credits() const
{
   return mCreditList;
}

const QValueList<KAboutTranslator>
KAboutData::translators() const
{
    QValueList<KAboutTranslator> personList;

    if(d->translatorName == 0)
        return personList;

    QStringList nameList;
    QStringList emailList;

    QString names = i18n(d->translatorName);
    if(names != QString::fromUtf8(d->translatorName))
    {
#if QT_VERSION < 0x040000
        nameList = QStringList::split(',',names);
#else
        nameList = names.split(',', QString::SkipEmptyParts);
#endif
    }


    if(d->translatorEmail)
    {
        QString emails = i18n(d->translatorEmail);

        if(emails != QString::fromUtf8(d->translatorEmail))
        {
#if QT_VERSION < 0x040000
            emailList = QStringList::split(',',emails,true);
#else
            emailList = emails.split(',');
#endif
        }
    }


    QStringList::Iterator nit;
    QStringList::Iterator eit=emailList.begin();

    for(nit = nameList.begin(); nit != nameList.end(); ++nit)
    {
        QString email;
        if(eit != emailList.end())
        {
            email=*eit;
            ++eit;
        }

        QString name=*nit;

#if QT_VERSION < 0x040000
        personList.append(KAboutTranslator(name.stripWhiteSpace(), email.stripWhiteSpace()));
#else
        personList.append(KAboutTranslator(name.trimmed(), email.trimmed()));
#endif
    }

    return personList;
}

QString
KAboutData::aboutTranslationTeam()
{
    return i18n("replace this with information about your translation team",
            "<p>KDE is translated into many languages thanks to the work "
            "of the translation teams all over the world.</p>"
            "<p>For more information on KDE internationalization "
            "visit http://i18n.kde.org</p>");
}

QString
KAboutData::otherText() const
{
   if (mOtherText && *mOtherText)
      return i18n(mOtherText);
   else
      return QString::null;
}


QString
KAboutData::license() const
{
  QString result;
  if (!copyrightStatement().isEmpty())
    result = copyrightStatement() + "\n\n";

  QString l;
  QString f;
  switch ( mLicenseKey )
  {
    case License_File:
       f = QFile::decodeName(mLicenseText);
       break;
    case License_GPL_V2:
       l = "GPL v2";
       f = locate("data", "LICENSES/GPL_V2");
       break;
    case License_LGPL_V2:
       l = "LGPL v2";
       f = locate("data", "LICENSES/LGPL_V2");
       break;
    case License_BSD:
       l = "BSD License";
       f = locate("data", "LICENSES/BSD");
       break;
    case License_Artistic:
       l = "Artistic License";
       f = locate("data", "LICENSES/ARTISTIC");
       break;
    case License_QPL_V1_0:
       l = "QPL v1.0";
       f = locate("data", "LICENSES/QPL_V1.0");
       break;
    case License_Custom:
       if (mLicenseText && *mLicenseText)
          return( i18n(mLicenseText) );
       // fall through
    default:
       result += i18n("No licensing terms for this program have been specified.\n"
                   "Please check the documentation or the source for any\n"
                   "licensing terms.\n");
       return result;
      }

  if (!l.isEmpty())
     result += i18n("This program is distributed under the terms of the %1.").arg( l );

  if (!f.isEmpty())
  {
     QFile file(f);
#if QT_VERSION < 0x040000
     if (file.open(IO_ReadOnly))
#else
     if (file.open(QIODevice::ReadOnly))
#endif
     {
        result += '\n';
        result += '\n';
        QTextStream str(&file);
#if QT_VERSION < 0x040000
        result += str.read();
#else
        result += str.readAll();
#endif
     }
  }

  return result;
}

QString
KAboutData::copyrightStatement() const
{
  if (mCopyrightStatement && *mCopyrightStatement)
     return i18n(mCopyrightStatement);
  else
     return QString::null;
}
