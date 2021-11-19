using System;
using System.Net;
using System.Security.Cryptography.X509Certificates;

namespace CheckCert
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("CheckCert.exe https://www.nytimes.com");
                Environment.Exit(0);
            }

            string url = args[0];

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
            HttpWebResponse response = null;
            X509Certificate2 cert = null;
            request.Timeout = 2000;

            try
            {
                response = (HttpWebResponse)request.GetResponse();
                response.Close();

                //retrieve the ssl cert and assign it to an X509Certificate object
                X509Certificate c = request.ServicePoint.Certificate;

                //convert the X509Certificate to an X509Certificate2 object by passing it into the constructor - https://stackoverflow.com/questions/15270764/get-ssl-certificate-in-net
                cert = new X509Certificate2(c);
            }
            catch
            {
                Console.WriteLine("[!] Unable to check SSL certificate on: " + url);
                Environment.Exit(1);
            }

            if (cert != null)
            {
                Console.WriteLine("[+] Certificate for: " + url + "\n");
                Console.WriteLine("Name: " + cert.Subject);
                Console.WriteLine("Issuer: " + cert.Issuer);
                Console.WriteLine("Expiration: " + cert.GetExpirationDateString());
                Console.WriteLine("Effective Date: " + cert.GetEffectiveDateString());
                Console.WriteLine("Thumbprint: " + cert.GetCertHashString());
                Console.WriteLine("Serial Number: " + cert.GetSerialNumberString());
                Console.WriteLine("Public Key String: " + cert.GetPublicKeyString());
            }
            else
            {
                Console.WriteLine("[!] Unable to check SSL certificate on: " + url);
            }
        }
    }
}
