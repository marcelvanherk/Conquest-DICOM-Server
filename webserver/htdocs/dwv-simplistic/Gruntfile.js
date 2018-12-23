/* global module */
module.exports = function(grunt) {
    // Project configuration
    grunt.initConfig({
        pkg: grunt.file.readJSON('package.json'),
        jshint: {
            files: ['Gruntfile.js', 'service-worker.js', 'src/*.js'],
            options: {
                jshintrc: '.jshintrc'
            }
        },
        copy: {
            dev: {
                src: 'index.html',
                dest: 'index-dev.html',
                options: {
                    process: function (content/*, srcpath*/) {
                        // do not register service worker
                        return content.replace(/<script type="text\/javascript" src="src\/register-sw\.js"><\/script>/g, 
                            '<!-- <script type="text/javascript" src="src/register-sw.js"></script> -->');
                    },
                },
            },
        },
        connect: {
            prod: {
                options: {
                    port: 8080,
                    hostname: 'localhost',
                    livereload: true
                }
            },
            dev: {
                options: {
                    port: 8080,
                    hostname: 'localhost',
                    livereload: true,
                    base: {
                        path: './',
                        options: {
                            index: 'index-dev.html'
                        }
                    }
                }
            }
        },
        watch: {
            scripts: {
                files: ['**/*.js', '!**/node_modules/**', '**/node_modules/dwv/**', '*.html'],
                options: {
                    spawn: false,
                    livereload: true
                }
            }
        }
    });

    grunt.loadNpmTasks('grunt-contrib-copy');
    grunt.loadNpmTasks('grunt-contrib-connect');
    grunt.loadNpmTasks('grunt-contrib-jshint');
    grunt.loadNpmTasks('grunt-contrib-watch');

    // Task to run tests
    grunt.registerTask('test', ['jshint']);
    grunt.registerTask('start', ['connect:prod', 'watch']);
    grunt.registerTask('dev', ['copy:dev', 'connect:dev', 'watch']);
};
