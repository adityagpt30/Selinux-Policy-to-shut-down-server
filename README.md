# Selinux Policy to shut down server
SELinux is a Mandatory Access Control solution for Linux. Under targeted SELinux, certain high-risk applications (ssh, Apache, sendmail, etc.) are placed in domains. A domain's privileges are restricted to no more than what is required to run the process contained in that domain. As a result, should a vulnerability in a high-risk application be exploited, the ensuing damage is limited to the domain in which that application executes. Developed and demonstrated the effectiveness of an SELinux policy module that sandboxes a (potentially vulnerable) server. 
 # Task
 Used SELinux to implement the principle of least privilege by sandboxing a server to a particular domain (i.e. set of privileges). It Shows how SELinux prohibits any server action not expressly permitted by the server's security policy and also it shows how SELinux shuts down the server when a security policy violation is detected. Developed, installed and demonstrated a SELinux policy module that runs the server program in its own domain. 
 # About video
 This video demonstrates how SELinux sandboxes the server: The video shows:

* The unconfined server running in an unconfined domain.
* a successful "attack" against the unconfined server (simply simulated by selecting the attack option).
* how the unconfined server can listen on any port.
* the sandboxed server running in its own constrained domain.
* how the sandboxed server prevents the attack.
* how the sandboxed server is restricted to operating only on port 8001.

# Approach
* First needed a working SELinux installation virtual machine. 
* Researched on how to build and install a policy module. The online resources and the documentation that comes with the SELinux packages helped a lot. There are also numerous SELinux policy helper and troubleshooting packages that you have to install.
* After building simple module, started working on sandboxing the server program. Put the program in an empty domain (i.e. create a policy that, apart from transitioning the server to a new domain, provides no privileges) and run it. You should see SELinux report the errors that arise when the program attempts disallowed operations. For each error that arises you will need to add the necessary rule to your policy and iterate until SELinux complains no more and the server program executes correctly. (You may find the audit2allow tool useful for generating the necessary rules to add to your policy.)
