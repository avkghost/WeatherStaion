#! /usr/bin/env perl

use strict;
use vars qw(@ISA $loaded);

use RRDs;
use LWP::UserAgent;

my $ua = LWP::UserAgent->new;

use LWP::Simple qw($ua get);

$ua->timeout(30);

my $start=time;
my $vhost="/home/linux/virtualhosts/sensors";

my $img="$vhost/www";
my $sensorURL="http://192.168.1.100/";

my $content = get($sensorURL);
my $room;
my $humidity;
my $temp;
my $heatIdx;

($room, $humidity, $temp, $heatIdx) = $content =~ /Room: ([\d+\.]*)\s+
Humidity: ([\d+\.]*).*
Temperature: ([\d+\.]*).*
Heat index: ([\d+\.]*)/mi;

my $rrd="$vhost/cgi-bin/room$room.rrd";

my $ERROR = RRDs::error;

print "$start:$humidity:$temp:$heatIdx\n";

RRDs::update $rrd, "$start:$humidity:$temp:$heatIdx";
if ($ERROR = RRDs::error) {
    die "$0: unable to update `$rrd': $ERROR\n";
}

RRDs::graph "$img/room$room-day.png",
  "--title", "Room $room", 
  "--start", "end-1d",
  "--interlace", 
  "--imgformat","PNG",
  "--width=600",
  "DEF:humidity=$rrd:humidity:LAST",
  "DEF:temperature=$rrd:temperature:LAST",
  "DEF:heatidx=$rrd:heatidx:LAST",
  "AREA:humidity#0022e9: Humidity",
  "AREA:temperature#00b674: Temperature",
  "LINE1:heatidx#ff4400: Heat Index\\c",
  "GPRINT:humidity:LAST:Humidity %3.2lf%%",
  "GPRINT:temperature:LAST:Temperature %3.2lf°C",
  "GPRINT:heatidx:LAST:Heat Index %3.2lf°C";

if ($ERROR = RRDs::error) {
  die "ERROR: $ERROR\n";
};

RRDs::graph "$img/room$room-week.png",
  "--title", "Room $room", 
  "--start", "end-7d",
  "--interlace", 
  "--imgformat","PNG",
  "--width=600",
  "DEF:humidity=$rrd:humidity:AVERAGE",
  "DEF:temperature=$rrd:temperature:AVERAGE",
  "DEF:heatidx=$rrd:heatidx:AVERAGE",
  "AREA:humidity#0022e9: Humidity",
  "AREA:temperature#00b674: Temperature",
  "LINE1:heatidx#ff4400: Heat Index\\c",
  "GPRINT:humidity:AVERAGE:Humidity %3.2lf%%",
  "GPRINT:temperature:AVERAGE:Temperature %3.2lf°C",
  "GPRINT:heatidx:AVERAGE:Heat Index %3.2lf°C";

if ($ERROR = RRDs::error) {
  die "ERROR: $ERROR\n";
};

RRDs::graph "$img/room$room-month.png",
  "--title", "Room $room", 
  "--start", "end-1m",
  "--interlace", 
  "--imgformat","PNG",
  "--width=600",
  "DEF:humidity=$rrd:humidity:AVERAGE",
  "DEF:temperature=$rrd:temperature:AVERAGE",
  "DEF:heatidx=$rrd:heatidx:AVERAGE",
  "AREA:humidity#0022e9: Humidity",
  "AREA:temperature#00b674: Temperature",
  "LINE1:heatidx#ff4400: Heat Index\\c",
  "GPRINT:humidity:AVERAGE:Humidity %3.2lf%%",
  "GPRINT:temperature:AVERAGE:Temperature %3.2lf°C",
  "GPRINT:heatidx:AVERAGE:Heat Index %3.2lf°C";

if ($ERROR = RRDs::error) {
  die "ERROR: $ERROR\n";
};

RRDs::graph "$img/room$room-year.png",
  "--title", "Room $room", 
  "--start", "end-1y",
  "--interlace", 
  "--imgformat","PNG",
  "--width=600",
  "DEF:humidity=$rrd:humidity:AVERAGE",
  "DEF:temperature=$rrd:temperature:AVERAGE",
  "DEF:heatidx=$rrd:heatidx:AVERAGE",
  "AREA:humidity#0022e9: Humidity",
  "AREA:temperature#00b674: Temperature",
  "LINE1:heatidx#ff4400: Heat Index\\c",
  "GPRINT:humidity:AVERAGE:Humidity %3.2lf%%",
  "GPRINT:temperature:AVERAGE:Temperature %3.2lf°C",
  "GPRINT:heatidx:AVERAGE:Heat Index %3.2lf°C";

if ($ERROR = RRDs::error) {
  die "ERROR: $ERROR\n";
};
