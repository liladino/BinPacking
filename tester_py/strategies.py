def sort_by_volume_desc(items):
    return sorted(items, key=lambda item: item[0] * item[1] * item[2], reverse=True)

def sort_by_largest_face_desc(items):
    def max_face_area(item):
        dims = sorted(item, reverse=True)
        return dims[0] * dims[1]
    return sorted(items, key=max_face_area, reverse=True)

def sort_by_longest_edge_desc(items):
    return sorted(items, key=lambda item: max(item), reverse=True)