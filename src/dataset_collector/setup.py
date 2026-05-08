from setuptools import setup
package_name = 'dataset_collector'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='root',
    maintainer_email='root@todo.todo',
    description='Automatic dataset collector',
    license='TODO',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'collector_node = dataset_collector.collector_node:main',
        ],
    },
)