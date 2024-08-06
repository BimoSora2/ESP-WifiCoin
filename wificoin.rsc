Mas BeBe # Konfigurasi interface
/interface bridge
add name=bridge-hotspot
/interface wireless
set [ find default-name=wlan1 ] mode=ap-bridge ssid=WiFiCoin
/interface bridge port
add bridge=bridge-hotspot interface=ether2
add bridge=bridge-hotspot interface=wlan1

# Konfigurasi address
/ip address
add address=10.5.50.1/24 interface=bridge-hotspot network=10.5.50.0

# Konfigurasi pool
/ip pool
add name=hs-pool ranges=10.5.50.2-10.5.50.254

# Konfigurasi DHCP server
/ip dhcp-server
add address-pool=hs-pool disabled=no interface=bridge-hotspot name=dhcp-hotspot
/ip dhcp-server network
add address=10.5.50.0/24 gateway=10.5.50.1

# Konfigurasi hotspot
/ip hotspot profile
add hotspot-address=10.5.50.1 name=hsprof1
/ip hotspot
add address-pool=hs-pool disabled=no interface=bridge-hotspot name=hotspot1 profile=hsprof1

# Konfigurasi user profile
/ip hotspot user profile
add name=1hour rate-limit=1M/2M session-timeout=1h

# Konfigurasi Firewall NAT
/ip firewall nat
add action=masquerade chain=srcnat out-interface=ether1

# Konfigurasi DNS
/ip dns
set allow-remote-requests=yes

# Konfigurasi API
/ip service
set api disabled=no port=8728
set api-ssl disabled=no port=8729

# Menambahkan user untuk API access
/user group
add name=api policy=api,read,write,policy,test
/user
add group=api name=apiuser password=StrongAPIPassword

# Konfigurasi Hotspot untuk menggunakan HTTPS
/ip hotspot profile
set hsprof1 login-by=https,http

# Mengaktifkan logging untuk hotspot
/system logging
add action=disk prefix=HOTSPOT topics=hotspot,info,debug
