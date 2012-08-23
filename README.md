[![build status](https://secure.travis-ci.org/deanmao/bloom.png)](http://travis-ci.org/deanmao/bloom)
Bloom Filters for Node
=======================

This is a simple node binding of Arash Partow's wonderful bloom filter library.

Example Usage
-------------

    npm install bloom

### Quick example:

    var BloomFilter = require('bloom').BloomFilter;
    var mytester = new BloomFilter(1000);
    
    mytester.insert('apple');
    mytester.insert('orange');
    
    if (mytester.contains('apple')) {
      console.log('apple is in our bit table');
    }
    
    if (!mytester.contains('squash')) {
      console.log('squash is not in our bit table');
    }
