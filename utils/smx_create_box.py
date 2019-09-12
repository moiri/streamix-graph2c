#!/usr/bin/env python

"""Prepare a box implementation from template files"""

__author__ = "Simon Maurer"
__version__ = "0.1.0"
__maintainer__ = "Simon Maurer"
__email__ = "tpf@humdek.unibe.ch"
__status__ = "Prototype"

import igraph
import sys, argparse
import os
import shutil
import fileinput
import textwrap

sys.settrace
parser = argparse.ArgumentParser('Prepare a box implementation from template files')
parser.add_argument( '-t', '--tpl', metavar="PATH", dest='tpl', default='/opt/smx/tpl/box', help='path to the streamix template folder' )
args = parser.parse_args()

def main():
    """main program entry point"""

    print textwrap.dedent("""\
        Enter the TYPE and the NAME of the box to be created.
         - The TYPE of a box is intended to describe the basic functionnality of
           the box. Use "src" for a source (only outputs), "snk" for a sink
           (only inputs), and "prc" for a process (inputs and putputs).
         - The NAME should be unique but simple and describe the box
           implementation well.
        The complete name of the box will be 'smx_TYPE_NAME'.
        """)

    b_type = raw_input("TYPE: ")
    b_name = raw_input("NAME: ")
    box_name = "smx_" + b_type + "_" + b_name
    lib_name = "smx" + b_type + b_name
    folder_name = "git-" + box_name

    print textwrap.dedent("""\
        Define the box signature, i.e the INPUT PORTS and the OUTPUT PORTS as
        a list of comma-seperated names.
        """)

    ports_in = raw_input("INPUT PORTS: ")
    ports_out = raw_input("OUTPUT PORTS: ")

    os.mkdir(folder_name)
    os.mkdir(folder_name + "/src")
    os.mkdir(folder_name + "/include")
    os.mkdir(folder_name + "/debian")
    os.mkdir(folder_name + "/test")
    copy = {
        "/README_md": "/README.md",
        "/box_mk": "/Makefile",
        "/config_mk": "/config.mk",
        "/box_json": "/box.json",
        "/_gitignore": "/.gitignore",
        "/src/box_c": "/src/" + box_name + ".c",
        "/include/box_h": "/include/" + box_name + ".h",
        "/include/box_sig_h": "/include/" + box_name + "_sig.h",
        "/debian/control": "/debian/control",
        "/debian/control-dev": "/debian/control-dev",
        "/debian/triggers": "/debian/triggers",
        "/debian/postinst": "/debian/postinst",
        "/debian/postrm": "/debian/postrm",
        "/test/main_c": "/test/test.c",
        "/test/test_c": "/test/test_" + box_name + ".c",
        "/test/test_h": "/test/test_" + box_name + ".h",
        "/test/test_mk": "/test/Makefile",
        "/test/test_json": "/test/test.json",
        "/test/test_zlog": "/test/test.zlog"
    }
    replace = {
        '<box_ports_in>': smx_ports(box_name, ports_in.split(','), 'in',
            args.tpl + "/include/tpl_port_sig_h", {
                    '<box_name>': box_name,
                    '<port_name>': "EMPTY",
                    '<port_mode>': "in"
                }),
        '<box_ports_out>': smx_ports(box_name, ports_out.split(','), 'out',
            args.tpl + "/include/tpl_port_sig_h", {
                    '<box_name>': box_name,
                    '<port_name>': "EMPTY",
                    '<port_mode>': "out"
                }),
        '<net_ch_in>': smx_ports(box_name, ports_in.split(','), 'in',
            args.tpl + "/test/tpl_ch_c"),
        '<net_ch_out>': smx_ports(box_name, ports_out.split(','), 'out',
            args.tpl + "/test/tpl_ch_c", None, len(ports_in)),
        '<net_ch_rm_in>': smx_ports(box_name, ports_in.split(','), 'in',
            args.tpl + "/test/tpl_ch_rm_c"),
        '<net_ch_rm_out>': smx_ports(box_name, ports_out.split(','), 'out',
            args.tpl + "/test/tpl_ch_rm_c", None, len(ports_in)),
        '<box_ports_in_case>': smx_ports(box_name, ports_in.split(','), 'in',
            args.tpl + "/test/tpl_port_c"),
        '<box_ports_out_case>': smx_ports(box_name, ports_out.split(','), 'out',
            args.tpl + "/test/tpl_port_c"),
        '<box_ports_in_conf>': smx_ports(box_name, ports_in.split(','), 'in',
            args.tpl + "/test/tpl_port_json"),
        '<box_ports_out_conf>': smx_ports(box_name, ports_out.split(','), 'out',
            args.tpl + "/test/tpl_port_json"),
        '<box_name>': box_name,
        '<lib_name>': lib_name,
        '<in_degree>': str(len(ports_in)),
        '<out_degree>': str(len(ports_out)),
    }

    for src, tgt in copy.items():
        shutil.copyfile(args.tpl + src, folder_name + tgt)
        sed(folder_name + tgt, replace)

    os.chmod(folder_name + "/debian/postinst", 0775);
    os.chmod(folder_name + "/debian/postrm", 0775);

    print textwrap.dedent("""\

        To change compilation options edit the file
        - '""" + folder_name + """/config_mk'

        To implement the box functionality edit the files
        - '""" + folder_name + """/src/""" + box_name + """.c'.
        - '""" + folder_name + """/include/""" + box_name + """.h'.
        - '""" + folder_name + """/include/""" + box_name + """_sig.h'.

        To document your implementation edit the file
        - '""" + folder_name + """/README.md'

        Prepare a default configuration file by editing the file
        - '""" + folder_name + """/box.json'

        To prepare a debian package change the following items:
        - 'Maintainer' in '""" + folder_name + """/debian/control'
        - 'Description' in '""" + folder_name + """/debian/control'
        - 'Maintainer' in '""" + folder_name + """/debian/control-dev'

        DO NOT TOUCH ANY TAG! They will be automatically set when running
        the command 'make dpkg'.

        """)

def sed(src, values):
    for line in fileinput.input(src, inplace=True):
        # inside this loop the STDOUT will be redirected to the file
        # the comma after each print statement is needed to avoid double line breaks
        for key, value in values.items():
            line = line.replace(key, value)
        print line,

def smx_ports(box_name, names, mode, tgt, default = None, offset = 0):
    res = "";
    idx = 0;

    for name in names:
        if name == "":
            continue
        res += smx_replace(tgt, {
                '<box_name>': box_name,
                '<port_name>': name.strip(),
                '<port_mode>': mode,
                '<ch_id>': str(idx + offset)
            })
        idx += 1

    if res == "" and default is not None:
        return smx_replace(tgt, default).strip()
    return res.strip()

def smx_replace(src, values):
    res = ""
    for line in fileinput.input(src):
        for key, value in values.items():
            line = line.replace(key, value)
        res += line
    return res

if __name__ == "__main__":
    main()
