import numpy as np
import pandas as pd
import scipy.signal
import matplotlib.pyplot as plt

# Files and import functions

files_miniT = ['Tests/minit_data/test1.txt',
               'Tests/minit_data/test2.txt',
               'Tests/minit_data/test3.txt',
               'Tests/minit_data/test4.txt',
               'Tests/minit_data/test5.txt',
               'Tests/minit_data/test6.txt', ]

files_cherT_pla = ['microT/PLA-01/0',
                   'microT/PLA-02/0',
                   'microT/PLA-03/0', ]

files_cherT_tape = ['microT/Tape/0',
                    'microT/tape02/0',
                    'microT/tape03/0', ]


def import_null_files_to_dfs(files, filter_force=False):
    dfs = []
    for file in files:
        df = pd.read_csv(file, sep='\t', decimal=',')
        df['strain'] = (df['Extensiometre (mm)'] - df['Extensiometre (mm)'].min()) / df['Extensiometre (mm)']

        if filter_force:
            window = df.shape[0] // 10
            if (window % 2) == 0:
                window += 1
            df['force_filt'] = scipy.signal.savgol_filter(df['Force DDS (N)'], window, 1)

        dfs.append(df)

    return dfs


def import_txt_files_to_dfs(files):
    """
    imports the files into a list of dataframes
    """
    dfs = []
    for file in files:
        df = pd.read_csv(file, sep=' ', names=['distance', 'force'])
        window = df['distance'].shape[0] // 5
        if (window % 2) == 0:
            window += 1
        df['dist_filt'] = scipy.signal.savgol_filter(df['distance'], window, 1)
        # start at force = 2N
        try:
            index = np.where(df['force'] > 2)[0][0]
            df = df.drop(np.arange(index))
        except IndexError:
            pass

        df['dist_filt'] = (df['dist_filt'] - df['dist_filt'].min()) / df['dist_filt'].min()
        dfs.append(df)

    return dfs


def strange(i):
    return [str(i) for i in np.arange(i)]


# ## Mini tensile data

# In[39]:


dfs = import_txt_files_to_dfs(files_miniT)
fig, axs = plt.subplots(len(dfs) // 2, 2, figsize=(16, 10))
axs = axs.reshape(-1)

for df, ax, i in zip(dfs, axs, range(6)):
    ax.plot(df['dist_filt'], df['force'])
    ax.set_title('test no {}'.format(i))
    ax.grid('on')
    ax.set_xlabel('distance (mm)')
    ax.set_ylabel('force (N)')

axs[0].set_title('test avec PLA 3D')
axs[1].set_title('Sliding tape')
axs[2].set_title('Sliding tape')
axs[3].set_title('Tape')
axs[4].set_title('Tape')
axs[5].set_title('Tape')

plt.tight_layout()
plt.show()

# ## Cher Tensile PLA data.ipynb_checkpoints/

# In[43]:


dfs_pla = import_null_files_to_dfs(files_cherT_pla)

fig, axs = plt.subplots(3, 1, figsize=(10, 10), sharex=True)
axs = axs.reshape(-1)

for df, ax, i in zip(dfs_pla, axs, range(len(dfs_pla))):
    ax.plot(df['Deplacement DDS (mm)'], df['Force DDS (N)'] / (0.26 * 5.92))
    ax.set_title('test pla #{}'.format(i))
    ax.grid('on')
    ax.set_xlabel('distance (mm)')
    ax.set_ylabel('Stress (MPa)')

plt.tight_layout()
plt.show()

# ## Cher Tensile Tape data

# In[8]:


dfs_tape = import_null_files_to_dfs(files_cherT_tape, filter_force=True)

fig, axs = plt.subplots(3, 1, figsize=(10, 10), sharex=True)
axs = axs.reshape(-1)

for df, ax, i in zip(dfs_tape, axs, range(len(dfs_pla))):
    ax.plot(df['Deplacement DDS (mm)'], df['force_filt'])
    ax.set_title('test tape #{}'.format(i))
    ax.grid('on')
    ax.set_xlabel('distance (mm)')
    ax.set_ylabel('force (N)')

plt.tight_layout()
plt.show()

# ## Tape data compare

# In[176]:


# minitensile tape
minit_tape = dfs[-3:]
fig, ax = plt.subplots(figsize=(8, 6))

for i, df in enumerate(minit_tape):
    if i == 0:
        ax.plot(df['dist_filt'] * 20, df['force'], c='r', label='MiniT')
    else:
        ax.plot(df['dist_filt'] * 20, df['force'], c='r')

for i, df in enumerate(dfs_tape):
    if i == 0:
        ax.plot(df['Deplacement DDS (mm)'], df['force_filt'], c='b', label='CherT')
    else:
        ax.plot(df['Deplacement DDS (mm)'], df['force_filt'], c='b')
ax.grid('on')
ax.set_xlabel('~strain')
ax.set_ylabel('force (N)')
ax.legend()
ax.set_title('Tape comparison')
plt.show()

# ## TODO
# - Test mini tensile même vitesse
# - Test mini tensile PLA bonne section

# ## PLA tests on mini T

# In[47]:


pla_mini_files = ['Tests/pla_minit/pla_mini_1.txt',
                  'Tests/pla_minit/pla_mini_2.txt', ]
L0 = 107  # mm
a = 0.26
b = 5.92
dfs = []

for file in pla_mini_files:
    df = pd.read_csv(file, sep=' ', names=['D', 'F'])

    if df['D'].shape[0] // 5 % 2 == 0:
        filt_size = df['D'].shape[0] // 5 + 1
    else:
        filt_size = df['D'].shape[0] // 5

    df['DF'] = scipy.signal.savgol_filter(df['D'], filt_size, 1)

    df['A'] = df['DF'] - df['DF'].min()
    df['eps'] = (L0 - df['A']) / L0
    df['sigma'] = df['F'] / (a * b)
    dfs.append(df)

fig, axs = plt.subplots(4, 1, figsize=(7, 16))

for df in dfs:
    axs[0].plot(df['eps'][:df['eps'].shape[0] - 120], df['sigma'][:df['eps'].shape[0] - 120])
    axs[1].plot(df['D'])
    axs[2].plot(df['eps'])
    axs[3].plot(df['sigma'])

axs[0].set_xlabel('strain')
axs[0].set_ylabel('stress')

axs[1].set_xlabel('index')
axs[1].set_ylabel('measured displacement')

axs[2].set_xlabel('index')
axs[2].set_ylabel('strain')

axs[3].set_xlabel('index')
axs[3].set_ylabel('stress')

axs[0].set_title('Wierd stress-strain curve')
axs[1].set_title('Unprecise raw data (displacement)')
axs[2].set_title('Filtered strain (still woobly)')
axs[3].set_title('Stress looks great')

axs[0].grid('on')
axs[1].grid('on')
axs[2].grid('on')
axs[3].grid('on')

plt.tight_layout()





