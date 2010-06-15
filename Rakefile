# -*- ruby -*-

require 'yaml'
require 'rubygems'

begin
  require 'hoe'
  gem 'rake-compiler', '>= 0.4.1'
  require "rake/extensiontask"

  Dir[File.expand_path('../tasks/**/*.rb',  __FILE__)].each do |file|
    require file
  end

  Hoe.spec 'mallcop' do
    developer('Aaron Patterson', 'aaron@tenderlovemaking.com')
    self.readme_file   = 'README.rdoc'
    self.history_file  = 'CHANGELOG.rdoc'
    self.extra_rdoc_files  = FileList['*.rdoc']
    self.spec_extras = { :extensions => ["ext/mallcop/extconf.rb"] }
    self.testlib = :minitest

    extra_dev_deps << ['rake-compiler', '>= 0.4.1']
    Rake::ExtensionTask.new "mallcop", spec do |ext|
      ext.lib_dir = File.join(*['lib', 'mallcop', ENV['FAT_DIR']].compact)
    end
  end

  Hoe.add_include_dirs('.')
  task :test => [:compile, "test:files"]
rescue LoadError
  STDERR.puts "`hoe` and `rake-compiler` are required to build the gem"
end

ROOT = File.expand_path('../', __FILE__)
# vim: syntax=ruby
