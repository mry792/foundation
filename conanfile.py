#! /usr/bin/env python3

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import update_conandata
from conan.tools.scm import Git
from conans.tools import Git as LegacyGit, get_env


class FoundationRecipe (ConanFile):
    name = 'foundation'
    # version = (computed from local repo)

    author = 'M. Emery Goss <m.goss792@gmail.com>'
    url = 'https://github.com/mry792/foundation.git'
    description = (
        'Collection of standard utilities. All support compiling for embedded '
        'systems.'
    )

    settings = 'compiler', 'build_type'
    generators = (
        'virtualenv',
        'CMakeDeps',
        'CMakeToolchain',
    )

    @property
    def _run_tests (self):
        return get_env('CONAN_RUN_TESTS', default = False)

    def package_id (self):
        self.info.clear()

    def set_version (self):
        git = LegacyGit(self.recipe_folder)
        tag = git.run('describe --tags')
        self.version = tag[1:]

    def build_requirements (self):
        if self._run_tests:
            self.test_requires('catch2/2.13.9')
            self.test_requires('trompeloeil/42')

    def export (self):
        git = Git(self, self.recipe_folder)
        scm_url, scm_commit = git.get_url_and_commit()
        update_conandata(self, {
            'source': {
                'commit': scm_commit,
                'url': scm_url,
            }
        })

    def source (self):
        git = Git(self)
        source = self.conan_data['source']
        git.clone(source['url'], target = '.')
        git.checkout(source['commit'])

    def layout (self):
        cmake_layout(self)

    def build (self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package (self):
        cmake = CMake(self)
        cmake.install()
