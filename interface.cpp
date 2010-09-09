#include "soapindexServiceObject.h"

struct Namespace namespaces[] =
{   // {"ns-prefix", "ns-name"}
   {"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/"}, // MUST be first
   {"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/"}, // MUST be second
   {"xsi", "http://www.w3.org/2001/XMLSchema-instance"}, // MUST be third
   {"xsd", "http://www.w3.org/2001/XMLSchema"}, // 2001 XML Schema
   {"ns1", "urn:xmethods-delayed-quotes"}, // given by the service description
   {"ns", "urn:driver"},
   {NULL, NULL} // end of table
}; 

int main()
{
   // create soap context and serve one CGI-based request:
  soap_serve(soap_new());

  /*
  indexServiceService service;
  service.serve();
  */
  return 0;
}

int ns__lookup(struct soap *soap, char * &response)
{
  response = "ciao";
  return SOAP_OK;
} 
