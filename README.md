# CheckCert
A small utility to request the SSL certificate from a public or private web application. CheckCert helps operators in the following ways:
1. It validates whether or not a webserver can be reached.
2. The `Issuer` field can help determine if SSL/TLS inspection is in place.
3. Hostnames can be obtained via the `Name` field in cases where IP addresses don't have associated PTR records.

Both a C# and BOF version of CheckCert are included. The BOF version was created to overcome an operational issue in an environment with tight egress rules. It was possible to establish DNS C2, however, it was difficult to find a suitable domain that was allowed outbound via HTTPS. The CheckCert BOF was created in an effort to minimize the amount of traffic sent via DNS, while providing the ability to request SSL certificates from publicly accessible domains.

## C# Usage
You can grab a copy of CheckCert from the [releases](https://github.com/skahwah/CheckCert/releases) page. Alternatively, feel free to compile the solution yourself.

```
CheckCert.exe https://nytimes.com
[+] Certificate for: https://nytimes.com

Name: CN=nytimes.com
Issuer: CN=Sectigo RSA Domain Validation Secure Server CA, O=Sectigo Limited, L=Salford, S=Greater Manchester, C=GB
Expiration: 4/5/2022 8:00:00 PM
Effective Date: 1/2/2020 7:00:00 PM
Thumbprint: CB29785052F1B91E530CBE546C11DFE62994D76E
Serial Number: 00B947803967139F666A54B56C27B852B5
Public Key String: 3082010A0282010100DAF21805C1248D18F129104486B7882B99DBC30948B3691A7383075DEBA1FD054173E0FDB31432BAE4924F311CA2D1312541A85C08F8B0D0FAF7F35454C1E0D3C1ADC679E274A39124A5A4BCA93B7DB3D4E682591D6CA363A26D350CD06951089BD249148A33DD46174B0112B42786E312A878D5A8EC6B181A2DEFC4384558597081D596B2711CB9728ED423FED2FDD4DF315BE3F278C37A36CBE7BB4788447FD54EC5598D446CFA17EDA8D4C18FE5067B9263FCF0FD768BFE24D59BDFA89719E53F26937E1D526F9A71938D7F099BFE85077AA278F5B98F1E4081FB47E22313E0D588956A058DDA7AF7DED0B68F0F1DE662D594D31CED164C94B16C7D0D34A70203010001
```

## BOF Usage
The BOF can take a comma-separated list of domains. Several assumptions have been made, which can easily be changed by editing the source file and recompiling:
1. The connect port has been set to 443.
2. The HTTP referrer has been set to `/`.
3. The `User-Agent` string has been has been set to `Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.69 Safari/537.36`.

You can grab a copy of the CheckCert BOF [here](https://github.com/skahwah/CheckCert/blob/main/CheckCert_BOF/CheckCertx64.o). Alternatively, feel free to compile yourself using either the x86 or x64 Developer Command Prompt for VS:
`cl.exe /c /GS- CheckCert.c /FoCheckCertx64.o`

```
inline-execute C:\Users\skawa\Desktop\CheckCertx64.o www.ft.com,www.cnn.com
[*] Tasked beacon to inline-execute C:\Users\skawa\Desktop\CheckCertx64.o
[+] host called home, sent: 3704 bytes
[+] received output:

[+] Getting SSL certificate details for https://www.ft.com:443/

[+] received output:

Name: CN=*.ft.com
Issuer: C=BE, O=GlobalSign nv-sa, CN=GlobalSign Atlas R3 DV TLS CA 2020
Expiration: 06/28/2022
Effective Date: 05/27/2021

[+] received output:

[+] Getting SSL certificate details for https://www.cnn.com:443/

[+] received output:

Name: CN=*.api.cnn.com
Issuer: C=BE, O=GlobalSign nv-sa, CN=GlobalSign Atlas R3 DV TLS CA 2020
Expiration: 05/22/2022
Effective Date: 04/20/2021
```

## Credits / References
- Having been a [PoshC2](https://github.com/nettitude/PoshC2) user for many years, a module I frequently used was [SSLInspectionCheck](https://github.com/nettitude/PoshC2/blob/master/resources/modules/SSLInspectionCheck.ps1). 
- [@anthemtotheego](https://twitter.com/anthemtotheego) and [@0xBoku](https://twitter.com/0xBoku) spent many hours answering the pedestrian questions I had about C and BOF development. Thank you both.
