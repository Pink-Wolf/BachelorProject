const DATABASE_URL = "http://localhost:5042/api/"

async function getJson(path) {
    const response = await fetch(path, {
        method: 'GET',
        cache: 'no-cache',
        headers: {
            'Content-Type': 'application/json',
        },
    })
    if (!response.ok) return null
    return await response.json()
}
async function postJson(path, data) {
    const response = await fetch(path, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
    })
    if (!response.ok) return null
    return await response.json()
}

export async function getProject(project) {
    const entity = await getJson(`${DATABASE_URL}${project}`)
    if (entity == null) return null
    return {
        ...entity,
        project: project,
    }
}
export async function getHeader(project, header) {
    const entity = await getJson(`${DATABASE_URL}${project}/${header}`)
    if (entity == null) return null
    return {
        ...entity,
        project: project,
        header: header,
    }
}
export async function getEntity(project, header, name) {
    const entity = await getJson(`${DATABASE_URL}${project}/${header}/${name}`)
    if (entity == null) return null
    return {
        ...entity,
        project: project,
        header: header,
        name: name,
    }
}

export async function postEntity({ project, header, name, ...data }) {
    return await postJson(`${DATABASE_URL}${data.project}/${data.header}/${data.name}`, data)
}

export async function getOverview() {
    return await getJson(`${DATABASE_URL}overview`)
}
