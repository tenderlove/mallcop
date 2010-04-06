# -*- ruby -*-

require 'rubygems'
require 'hoe'
gem 'rake-compiler', '>= 0.4.1'
require "rake/extensiontask"

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

namespace :test do
  file "test/support/sshd/host" do
    dir = File.expand_path("../test/support/sshd", __FILE__)
    FileUtils.mkdir_p dir
    sh "ssh-keygen -t dsa -f #{dir}/host -N ''"
  end

  task :files => "test/support/sshd/host"
end

Hoe.add_include_dirs('.')
task :test => [:compile, "test:files"]

# vim: syntax=ruby
