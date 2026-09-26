<div align="center">

# IoT Security

**Coursework · 42037 IoT Security · University of Technology Sydney · Autumn 2024**

![C](https://img.shields.io/badge/C-A8B9CC?logo=c&logoColor=black)
![Contiki](https://img.shields.io/badge/Contiki-2.7%20%2F%20Cooja-4B8BBE)
![Wireshark](https://img.shields.io/badge/Wireshark-1679A7?logo=wireshark&logoColor=white)
![Kali Linux](https://img.shields.io/badge/Kali%20Linux-557C94?logo=kalilinux&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-yellow)

[Contents](#contents) · [Getting started](#getting-started) · [Related repositories](#related-repositories)

</div>

My lab solutions and both lab tests from the IoT Security subject I took during an exchange semester at UTS. The first
half of the subject uses the Contiki OS and the Cooja network simulator: sensor node basics, RPL/UDP communication,
authenticated messages between nodes, and CoAP behind a border router. The second half covers attacks on IoT devices
with Kali Linux (port scanning, packet crafting, Bluetooth sniffing, web application vulnerabilities).

> [!NOTE]
> Unofficial study material, kept as submitted in 2024. Nothing is corrected, so answers can be wrong or incomplete.
> The repository is not developed further. The lab sheets and the starter code from the teaching staff are not
> included.

## Course

| | |
|---|---|
| Subject | 42037 IoT Security |
| Institution | University of Technology Sydney, School of Electrical and Data Engineering |
| Semester | Autumn 2024 (February to June), exchange semester from the University of Zurich |

## Contents

In each week, `de*` folders are the demonstrations from the lab and `ex*` folders are the exercises. A folder
usually holds the modified Contiki source (`*.c`, compiled `*.sky`), the Cooja simulation (`*.csc`), a radio capture
(`*.pcap`, open with Wireshark) and screenshots of the result.

| Folder | Topic |
|---|---|
| [`week-01/`](week-01/) | Contiki and Cooja basics: broadcast, LEDs, buttons, timers |
| [`week-02/`](week-02/) | RPL and UDP client/server communication, traffic capture with Wireshark |
| [`week-03/`](week-03/) | Authentication between sensor nodes over UDP (SHA-256 hash of the message) |
| [`week-04/`](week-04/) | Connecting the IoT network to the internet: RPL border router and CoAP (Erbium) server |
| [`week-04-practice-test/`](week-04-practice-test/) | Practice exercises for lab test 1 (broadcast, UDP, authentication, border router) |
| [`week-05-lab-test-1/`](week-05-lab-test-1/) | Lab test 1: code and simulations per question (`q1`–`q3`) and the submitted report ([docx](week-05-lab-test-1/labtest1-nicolas-huber.docx)) |
| Week 6 | No lab (study break) |
| Weeks 7–10 | Port scanning with Nmap, packet crafting with hping3, Bluetooth sniffing on a Raspberry Pi, SQL injection with sqlmap. Only the lab sheets existed, so there is no folder. |
| [`week-11-lab-test-2/`](week-11-lab-test-2/) | Lab test 2 on weeks 7–10: the submitted report ([docx](week-11-lab-test-2/labtest2-nicolas-huber.docx)) |

## Getting started

The labs ran in the **Instant Contiki 2.7** virtual machine with the Cooja simulator, which the course provided. The
code does not build outside that environment.

1. Download Instant Contiki 2.7 from the [Contiki project](https://sourceforge.net/projects/contiki/files/Instant%20Contiki/)
   and start it in VMware Workstation Player (the course used version 15).
2. Copy the `.c` files of an exercise into the Contiki folder that its `.csc` file names, for example
   `[CONTIKI_DIR]/examples/sky/` for week 1 or `examples/ipv6/rpl-udp/` for the UDP exercises.
3. Start Cooja from `contiki/tools/cooja`:

   ```bash
   ant run
   ```

4. Open the `.csc` file of the exercise (**File → Open simulation**) and start the simulation.

The `.pcap` files open directly in [Wireshark](https://www.wireshark.org/).

## Known issues

- I have not re-run the simulations for this release.
- The screenshot file names contain `&%` where the time stamp had colons.

## Related repositories

Other subjects from the same exchange semester at UTS:

| Subject | Repository |
|---|---|
| 32130 Fundamentals of Data Analytics | [data-analytics-uts](https://github.com/HuberNicolas/data-analytics-uts) |
| 42037 IoT Security | this repository |
| Python Programming for Data Processing | [python-data-processing-uts](https://github.com/HuberNicolas/python-data-processing-uts) |
| 32547 UNIX Systems Programming | [unix-systems-programming-uts](https://github.com/HuberNicolas/unix-systems-programming-uts) |

## Acknowledgements

- Labs by Manh Bui, School of Electrical and Data Engineering, UTS.
- The C sources are modified examples from [Contiki](https://github.com/contiki-os/contiki) (Swedish Institute of
  Computer Science, Matthias Kovatsch), licensed under the 3-clause BSD license. Their copyright headers are kept.

## License

My own changes and reports are licensed under the [MIT License](LICENSE). The Contiki example code keeps its
3-clause BSD license. `sha256.h` was handed out in class and is not covered by the MIT License. The questions quoted in
the lab test reports belong to the University of Technology Sydney.

## Author

Nicolas Huber ([@HuberNicolas](https://github.com/HuberNicolas)), exchange student at UTS in Autumn 2024.
