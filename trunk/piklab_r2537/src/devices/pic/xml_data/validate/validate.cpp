// Necessary includes. We refer to these as "common includes" 
// in the following examples.
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

// Handy definitions of constants.
#include <xercesc/util/XMLUni.hpp>

#include <iostream>

using namespace std;
XERCES_CPP_NAMESPACE_USE

class Handler : public DefaultHandler
{
public:
  virtual void  error (const SAXParseException &exc) {
    char* message = XMLString::transcode(exc.getMessage());
    cout << "Exception: " << message << "\n";
    XMLString::release(&message);
  }
  virtual void fatalError (const SAXParseException &exc) {
    char* message = XMLString::transcode(exc.getMessage());
    cout << "Exception: " << message << "\n";
    XMLString::release(&message);
  }
};

int main(int argc, char* argv[])
{
XMLPlatformUtils::Initialize();

// Create a SAX2 parser object.
SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();

// Set the appropriate features on the parser.
// Enable namespaces, schema validation, and the checking 
// of all Schema constraints.
// We refer to these as "common features" in following examples.
parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
parser->setFeature(XMLUni::fgXercesDynamic, false);
parser->setFeature(XMLUni::fgXercesSchema, true);
parser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
//parser->setProperty(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, (void *)"pic.xsd");

// Set appropriate ContentHandler, ErrorHandler, and EntityResolver.
// These will be referred to as "common handlers" in subsequent examples.

// You will use a default handler provided by Xerces-C++ (no op action).
// Users should write their own handlers and install them.
Handler handler;
parser->setContentHandler(&handler);

// The object parser calls when it detects violations of the schema.
parser->setErrorHandler(&handler);

// The object parser calls to find the schema and 
// resolve schema imports/includes.
parser->setEntityResolver(&handler);

// Parse the XML document.
// Document content sent to registered ContentHandler instance.
if ( argc==1 ) { printf("Needs one argument\n"); return -1; }
parser->parse(argv[1]);

// Delete the parser instance.
delete parser;

return 0;
}
