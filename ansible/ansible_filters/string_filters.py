def split_string(value, separator=' '):
    return value.split(separator)

class FilterModule(object):
    ''' String split filter '''

    def filters(self):
        return {
            'split': split_string
        }
