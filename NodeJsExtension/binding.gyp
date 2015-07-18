{
  "targets": [
    {
      "target_name": "greisjs",
      'link_settings': {
	    'libraries': [
	      '-lGreis',
	    ],
	    'library_dirs': [
	      '../build/lib/Debug',
	    ],
	  },
      'include_dirs': [
        "include",
      ],
      "sources": [
        "src/module.cpp"
      ]
    }
  ]
}
