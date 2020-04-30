# Selinux-Policy-to-shut-down-server
SELinux is a Mandatory Access Control solution for Linux. Under targeted SELinux, certain high-risk applications (ssh, Apache, sendmail, etc.) are placed in domains. A domain's privileges are restricted to no more than what is required to run the process contained in that domain. As a result, should a vulnerability in a high-risk application be exploited, the ensuing damage is limited to the domain in which that application executes.
