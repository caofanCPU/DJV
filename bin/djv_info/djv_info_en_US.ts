<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US">
<context>
    <name>djv::info::Application</name>
    <message>
        <location filename="InfoApplication.cpp" line="168"/>
        <source>Cannot open image: &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="184"/>
        <source>%1x%2:%3 %4 %5@%6</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="194"/>
        <location filename="InfoApplication.cpp" line="206"/>
        <location filename="InfoApplication.cpp" line="225"/>
        <source>%1 %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="201"/>
        <source>%1@%2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="213"/>
        <source>    %1. %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="219"/>
        <source>%1x%2:%3 %4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="237"/>
        <source>Layer = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="239"/>
        <source>  Width = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="241"/>
        <source>  Height = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="243"/>
        <source>  Aspect = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="247"/>
        <source>  Pixel = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="250"/>
        <source>Start = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="254"/>
        <source>End = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="258"/>
        <source>Duration = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="262"/>
        <source>Speed = %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="267"/>
        <source>Tag %1 = %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoApplication.cpp" line="294"/>
        <source>%1:</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>djv::info::Context</name>
    <message>
        <location filename="InfoContext.cpp" line="102"/>
        <source>-x_info</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="103"/>
        <source>-xi</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="108"/>
        <source>-verbose</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="109"/>
        <source>-v</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="114"/>
        <source>-file_path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="115"/>
        <source>-fp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="120"/>
        <source>-recurse</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="121"/>
        <source>-r</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="126"/>
        <source>-columns</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="127"/>
        <source>-c</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="InfoContext.cpp" line="149"/>
        <source>djv_info

    Print file metadata.

    Example output:
    yesterdayview.mov                    640x424:1.51 RGB U8 00:02:00:01@12
    dlad.dpx                          2048x1556:1.32 RGB U10 00:00:00:01@24
    render0001-1000.exr                                      00:00:41:16@24
        0: A,B,G,R                                     720x480:1.5 RGBA F16
        1: Ambient                                      720x480:1.5 RGB F16
        2: Diffuse                                      720x480:1.5 RGB F16
        3: Specular                                     720x480:1.5 RGB F16
        4: Z                                              720x480:1.5 L F32

    Key:
    (name)     (width)x(height):(aspect) (format) (type) (duration)@(speed)
    (name)                                               (duration)@(speed)
        (layer): (name)           (width)x(height):(aspect) (format) (type)

Usage

    djv_info [image|directory]... [option]...

    image     - One or more images, image sequences, or movies
    directory - One or more directories

    If no images or directories are given then the current directory will be used.

Options

    -x_info, -xi
        Don&apos;t show image information, only file names.
    -verbose, -v
        Show verbose image information.
    -file_path, -fp
        Show file path names.
    -recurse, -r
        Recursively descend into sub-directories.
    -columns, -c (value)
        Set the number of columns used to format the output. Setting this value to zero disables formatting.
%1
Examples

    Display image information:
    &gt; djv_info image.sgi image2.sgi

    Display image sequence information:
    &gt; djv_info image.1-100.sgi

    Display information about all images within a directory:
    &gt; djv_info ~/pics
</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
