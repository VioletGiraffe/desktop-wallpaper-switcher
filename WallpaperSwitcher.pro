TEMPLATE = subdirs

SUBDIRS = wpchanger_app wpchanger image qtutils cpputils cpp-template-utils

qtutils.depends = cpputils

wpchanger.depends = image qtutils

wpchanger_app.depends = image wpchanger qtutils
