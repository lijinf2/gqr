import sys

topK = 20
data_set = 'e2m_audio'
metric = 'product'

if len(sys.argv) > 3:
    topK = int(sys.argv[1])
    data_set = sys.argv[2]
    metric = sys.argv[3]
    print('use sys args parameter.\n topK\t: %d \n data set\t: %s \n metric\t: %s' % (topK, data_set, metric))
else:
    print('\033[95m use default parameter.\n topK\t: %d \n data set\t: %s \n metric\t: %s' % (topK, data_set, metric))

ground_truth = '%s_%s_groundtruth.lshbox' % (data_set, metric)
model_file = './hashingCodeTXT/KNNGraphmodel%s_%dk.txt' % (data_set.upper(), topK)


def extract_ids(line_txt):
    ids = []
    numbers = line_txt.split('\t')
    for i in range(topK):
        ids.append(int(numbers[2 * i + 1]))
    return ids


def read_nn(file_name):
    nn = None
    for line_txt in open(file_name):
        if nn is None:
            nn = []
        else:
            nn.append(extract_ids(line_txt))
    return nn


def write(file_name, knns):
    submission = open(file_name, 'w')
    for (i, knn) in enumerate(knns):
        submission.write('%d %s\n' % (i, ' '.join(map(str, knn))))
    submission.close()


knns = read_nn(ground_truth)
write(model_file, knns)
