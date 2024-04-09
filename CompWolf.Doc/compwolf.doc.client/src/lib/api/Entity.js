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
    const returnVal = {
        ...entity,
        project: project,
        header: header,
        name: name,
    }
    switch (entity.type) {
        case `class`: return { ...returnVal,
            constructor: {
                ...returnVal.constructor,
                briefDescription: `${returnVal.copyable ? "This can be copied." : "This cannot be copied."} ${returnVal.movable ? "This can be moved." : "This cannot be moved."}`,
                detailedDescription: `${returnVal.copyable ? "This can be copied." : "This cannot be copied."} ${returnVal.movable ? "This can be moved." : "This cannot be moved."}`
            }
        }
        default: return returnVal
    }

    
}

export async function getOverview() {
    return await getJson(`${DATABASE_URL}overview`)
}
export async function getPathTo(name) {
    const overview = await getOverview()

    var path = undefined
    overview.projects?.find(project => {
        if (project.name === name) path = `${project.name}/`
        project.headers?.find(header => {
            if (header.name === name) path = `${project.name}/${header.name}/`
            header.entities?.find(entity => {
                if (entity.name === name) path = `${project.name}/${header.name}/${entity.name}/`
                return (path !== undefined)
            })
            return (path !== undefined)
        })
        return (path !== undefined)
    })

    return path
}
