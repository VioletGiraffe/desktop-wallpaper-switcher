TEMPLATE = subdirs
CONFIG = ordered

SUBDIRS = sub_wpchanger sub_utility sub_image sub_qtutils sub_app

sub_wpchanger.subdir = wpchanger
sub_wpchanger.depends = image

sub_image.subdir = image

sub_utility.subdir = utility

sub_qtutils.subdir = qtutils

sub_app.subdir = wpchanger_app
sub_app.depends = sub_utility sub_image sub_wpchanger
