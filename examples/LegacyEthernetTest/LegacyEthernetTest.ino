
#include <Ethernet.h>
#include <NetApiHelpers.h>
#include <MACAddress.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };

void setup() {

  Serial.begin(115200);
  while (!Serial);
  Serial.println();

//	Ethernet.init(10);
//  Ethernet.begin(mac, INADDR_NONE); // force hw init for some libraries

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  Serial.println("Checking link ...");
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("\tretry...");
    delay(500);
  }
  switch (Ethernet.linkStatus()) {
    case LinkOFF:
      Serial.println("\tEthernet cable is not connected.");
      break;
    case LinkON:
      Serial.println("\tEthernet cable is connected.");
      break;
    default:
      Serial.println("\tLink state unknown.");
      break;
  }
  Serial.println();

  Serial.println("Attempting to connect with DHCP ...");
  Ethernet.setHostname("arduino");
  if (!Ethernet.begin(mac)) {
    Serial.println("\t...ERROR");
    while (true) {
      delay(1);
    }
  } else {
    Serial.println("\t...success");
  }
  Serial.println();

  printEthernetStatus();
  Serial.println();

  IPAddress ip = Ethernet.localIP();
  IPAddress gw = Ethernet.gatewayIP();
  IPAddress mask = Ethernet.subnetMask();
  IPAddress dns = Ethernet.dnsServerIP();

  Serial.println("Attempt to connect to port 80 on the gateway...");
  EthernetClient client;
  bool clientConnectOKwDHCP = client.connect(gw, 80);
  client.stop();
  if (clientConnectOKwDHCP) {
    Serial.println("\t...success");
  } else {
    Serial.println("\t...ERROR");
  }
  Serial.println();

  IPAddress dnsIP2(8, 8, 8, 8);
  Ethernet.setDnsServerIP(dnsIP2);
  if (Ethernet.dnsServerIP() != dnsIP2) {
    Serial.print("ERROR: DNS IP setter or getter error. dnsServerIP() returned ");
    Serial.println(Ethernet.dnsServerIP());
    Serial.println();
  }
  Ethernet.setDnsServerIP(dns);

  const char *serverName = "www.google.com";
  Serial.print("Resolve IP for ");
  Serial.println(serverName);
  IPAddress resolvedIP;
  if (Ethernet.hostByName("www.google.com", resolvedIP) == 1) {
    Serial.print("\t");
    Serial.println(resolvedIP);
  } else {
    Serial.println("\t...ERROR");
  }
  Serial.println();

  Ethernet.end();

//  IPAddress ip(192, 168, 1, 177);
//  IPAddress gw(192, 168, 1, 1);
//  IPAddress dns(192, 168, 1, 1);
//  IPAddress mask(255, 255, 255, 0);
//  bool clientConnectOKwDHCP = true;

  ip[3] = 177;

  Serial.print("Configuring static IP ");
  Serial.println(ip);
  Ethernet.begin(mac, ip, dns, gw, mask);
  if (Ethernet.dnsServerIP() == mask) {
    Serial.println("ERROR: begin() has wrong ordering of parameters");
    while (true) {
      delay(1);
    }
  }
  Serial.println();

  printEthernetStatus();
  Serial.println();

  if (ip != Ethernet.localIP()) {
    Serial.println("ERROR: Static IP was not used.");
    while (true) {
      delay(1);
    }
  }

  if (clientConnectOKwDHCP) {
    Serial.println("Attempt to connect to port 80 on the gateway...");
    EthernetClient client;
    if (client.connect(gw, 80)) {
      Serial.println("\t...success");
    } else {
      Serial.println("\t...ERROR");
    }
    client.stop();
    Serial.println();
  }

  Ethernet.end();

  ip[3] = 178;

  Serial.print("Configuring with automatic gateway, DNS and mask with static IP "); // <-------
  Serial.println(ip);
  Ethernet.begin(mac, ip);

  printEthernetStatus();
  Serial.println();

  if (ip != Ethernet.localIP()) {
    Serial.println("ERROR: Static IP was not used.");
    while (true) {
      delay(1);
    }
  }

  IPAddress autoIP(ip);
  autoIP[3] = 1;
  IPAddress defaultMask(255, 255, 255, 0);
  if (Ethernet.gatewayIP() == autoIP && Ethernet.dnsServerIP() == autoIP && Ethernet.subnetMask() == defaultMask) {
    Serial.println("Automatic config values are OK");
  } else {
    Serial.println("ERROR: Automatic config values are wrong");
    if (Ethernet.gatewayIP() != autoIP) {
      Serial.print("\tgateway IP Address: ");
      Serial.println(Ethernet.gatewayIP());
    }
    if (Ethernet.subnetMask() != defaultMask) {
      Serial.print("\tsubnet IP mask: ");
      Serial.println(Ethernet.subnetMask());
    }
    if (Ethernet.dnsServerIP() != autoIP) {
      Serial.print("\tDNS server: ");
      Serial.println(Ethernet.dnsServerIP());
    }
  }
  Serial.println();

  Ethernet.end();

  Serial.println("Attempting to connect with DHCP again ...");
  Ethernet.setHostname("arduino");
  if (!Ethernet.begin(mac)) {
    Serial.println("\t...ERROR");
    while (true) {
      delay(1);
    }
  } else {
    Serial.println("\t...success");
  }

  printEthernetStatus();
  Serial.println();

  if (ip == Ethernet.localIP()) {
    Serial.println("ERROR: The IP didn't change from static IP to DHCP assigned IP.");
  } else {
    Serial.println("Check on router the hostname and MAC address.");
  }
  Serial.println();

  Serial.println("END");
}

void loop() {
}

void printEthernetStatus() {

  MACAddress mac;
  Ethernet.MACAddress(mac);
  Serial.print("MAC: ");
  Serial.println(mac);
  if (mac[0] & 1) { // unicast bit is set
    Serial.println("\t is the ordering of the MAC address bytes reversed?");
  }

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());

  Serial.print("gateway IP Address: ");
  Serial.println(Ethernet.gatewayIP());

  Serial.print("subnet IP mask: ");
  Serial.println(Ethernet.subnetMask());

  Serial.print("DNS server: ");
  IPAddress dns = Ethernet.dnsServerIP();
  if (dns == INADDR_NONE) {
    Serial.println("not set");
  } else {
    Serial.println(dns);
  }
}
